// BPMS final code
// Authors: Andy Li, Stefan Dumitrescu, Mhamad Salih
// Copyright 2016, McMaster Solar Car Project
// Monitors voltage and performs charge balancing for 30 cells

// Includes
#include "main.h"
#include "math.h"
#include "pec.c"
#include "ltc6804.c"
#include "adc.c"
#include "fan.c"
#include "lcd.c"
#include "hall_sensor.c"
#include "eeprom.c"

// PIC internal register addresses
#word IFS0 = 0x0084

// Macros to disable timers and clear flags
#define CLEAR_T2_FLAG IFS0  &= 0xFF7F

// Telemetry packet IDs
#define VOLTAGE_ID  0x5A
#define TEMP_ID     0x69
#define BALANCE_ID  0x41
#define CURRENT_ID  0xE7

// Kilovac control
#define KILOVAC_ON  output_high(KVAC_PIN);
#define KILOVAC_OFF output_low(KVAC_PIN);

// Protection limits
#define VOLTAGE_MAX            42000 // 4.20V
#define VOLTAGE_MIN            27500 // 2.75V
#define TEMP_WARNING              60 // 60°C, actual value to be determined
#define TEMP_CRITICAL             70 // 70°C, actual value to be determined
#define CURRENT_DISCHARGE_LIMIT 1000 // -80A, actual value to be determined
#define CURRENT_CHARGE_LIMIT    4000 // +40A, actual value to be determined

// Status LED blink period
#define HEARTBEAT_PERIOD_MS 500

// Returns 1 only if voltage, temperature, and current are all within the safe
// operating ranges
#define SAFETY_CHECK (check_voltage() & check_temperature() & check_current())

static cell_t         g_cell[N_CELLS];
static unsigned int16 g_adc_data[N_ADC_CHANNELS];
static float          g_temps[N_ADC_CHANNELS];
static int            g_highest_voltage_cell_index;
static int            g_lowest_voltage_cell_index;
static int            g_highest_temperature_cell_index;
static unsigned int16 g_highest_voltage;
static unsigned int16 g_lowest_voltage;
static float          g_highest_temperature;
static unsigned int16 g_current;

// Timer 2 is used to send LabVIEW data
#int_timer2 level = 4
void isr_timer2(void)
{
    // Send data to LabVIEW over uart
    /*send_voltage_data();
    delay_ms(10);
    send_temperature_data();
    delay_ms(10);
    send_current_data();
    delay_ms(10);
    send_balancing_bits();*/
    output_toggle(STATUS);
}

// Initializes the cells, clears all flags, resets highest and lowest cells
void init_cells(void)
{
    int i;
    for (i = 0 ; i < N_CELLS ; i++)
    {
        g_cell[i].voltage = 0;
    }
    g_highest_voltage_cell_index = 0;
    g_lowest_voltage_cell_index = 0;
    g_highest_temperature_cell_index = 0;
    g_highest_voltage = 0;
    g_lowest_voltage = 0;
    g_highest_temperature = 0;
    g_current = 0;
}

// Returns the index for the highest voltage cell
int get_highest_voltage_cell_index(void)
{
    int i;
    int highest = 0;
    for (i = 0 ; i < N_CELLS ; i++)
    {
        if (g_cell[i].voltage >= g_cell[highest].average_voltage)
        {
            highest = i;
        }
    }
    return highest;
}

// Returns the index for the lowest voltage cell
int get_lowest_voltage_cell_index(void)
{
    int i;
    int lowest = 0;
    for (i = 0 ; i < N_CELLS ; i++)
    {
        if (g_cell[i].average_voltage <= g_cell[lowest].average_voltage)
        {
            lowest = i;
        }
    }
    return lowest;
}

int get_highest_temperature_cell_index(void)
{
    int i;
    int highest = 0;
    for (i = 0 ; i < N_ADC_CHANNELS ; i++)
    {
        if (g_temps[i] >= g_temps[highest])
        {
            highest = i;
        }
    }
    return highest;
}

// Use the simplified Steinhart-Hart equation to approximate temperatures
void convert_adc_data_to_temps(void)
{
    int i;
    float resistance;
    float temperature;
    for (i = 0; i < N_ADC_CHANNELS; i++)
    {
        resistance = THERMISTOR_SERIES * (float)g_adc_data[i] /
           (LSBS_PER_VOLT * THERMISTOR_SUPPLY - (float)g_adc_data[i]);
        temperature = resistance / THERMISTOR_NOMINAL;
        temperature = log(temperature);
        temperature /= B_COEFF;
        temperature += 1.0 / (TEMPERATURE_NOMINAL + 273.15);
        temperature = 1.0 / temperature;
        temperature -= 273.15;
        g_temps[i] = temperature;
    }
}

// Discharge all the cells that are 1% of the SoC range voltage higher than
// the lowest voltage
void balance(void)
{
    ltc6804_read_cell_voltages(g_cell);
    
    int i;
    int min_idx = get_lowest_voltage_cell_index();

    for (i = 0 ; i < 12 ; i++)
    {
        if ((g_cell[i].average_voltage - g_cell[min_idx].average_voltage)
            > BALANCE_THRESHOLD)
        {
            g_discharge1 |= 1 << i;
        }
        else
        {
            g_discharge1 &= ~(1 << i);
        }
    }

    for (i = 12 ; i < 24 ; i++)
    {
        if ((g_cell[i].average_voltage - g_cell[min_idx].average_voltage)
            > BALANCE_THRESHOLD)
        {
            g_discharge2 |= 1 << (i - 12);
        }
        else
        {
            g_discharge2 &= ~(1 << (i - 12));
        }
    }
    
    for (i = 24 ; i < 30 ; i++)
    {
        if ((g_cell[i].average_voltage - g_cell[min_idx].average_voltage)
            > BALANCE_THRESHOLD)
        {
            g_discharge3 |= 1 << (i - 24);
        }
        else
        {
            g_discharge3 &= ~(1 << (i - 24));
        }
    }

    output_low(CSBI1);
    ltc6804_write_config(g_discharge1);
    output_high(CSBI1);
    output_low(CSBI2);
    ltc6804_write_config(g_discharge2);
    output_high(CSBI2);
    output_low(CSBI3);
    ltc6804_write_config(g_discharge3);
    output_high(CSBI3);
}

void average_voltage(void)
{
    int i;
    int j;
    unsigned int32 sum;
    for (i = 0 ; i < N_CELLS ; i++)
    {
        sum = 0;
        for (j = 0 ; j < N_SAMPLES-1 ; j++)
        {
            sum += g_cell[i].samples[j];
            g_cell[i].samples[j] = g_cell[i].samples[j+1];
        }
        sum += g_cell[i].voltage;
        g_cell[i].samples[N_SAMPLES-1] = g_cell[i].voltage;
        g_cell[i].average_voltage = (unsigned int16) (sum/N_SAMPLES);
    }
}

void send_voltage_data(void)
{
    int i;
    putc(VOLTAGE_ID);
    for (i = 0 ; i < N_CELLS ; i ++)
    {
        putc((int8)(g_cell[i].average_voltage >> 8));
        putc((int8)(g_cell[i].average_voltage & 0x00FF));
    }
}

void send_temperature_data(void)
{
    int i;
    putc(TEMP_ID);
    for (i = 0 ; i < N_ADC_CHANNELS ; i++)
    {
        putc((unsigned int8)(g_temps[i]));
    }
}

void send_current_data(void)
{
    putc(CURRENT_ID);
    putc((int8)(g_current&0xFF));
    putc((int8)((g_current>>8)&0xFF));
}

void send_balancing_bits(void)
{
    int32 discharge = (g_discharge1&0x000FFF)
            |((int32)((g_discharge2&0x000FFF)<<12))
            |((int32)((g_discharge3&0x00003F)<<18));
    putc(BALANCE_ID);
    putc((int8)(discharge&0xFF));
    putc((int8)((discharge>> 8)&0xFF));
    putc((int8)((discharge>>16)&0xFF));
    putc((int8)((discharge>>24)&0x3F));
}

int1 check_voltage(void)
{
    // Find highest and lowest cell voltages
    ltc6804_read_cell_voltages(g_cell);
    average_voltage();
    g_highest_voltage_cell_index = get_highest_voltage_cell_index();
    g_lowest_voltage_cell_index = get_lowest_voltage_cell_index();
    g_highest_voltage = g_cell[g_highest_voltage_cell_index].average_voltage;
    g_lowest_voltage = g_cell[g_lowest_voltage_cell_index].average_voltage;
    
    if (g_highest_voltage >= VOLTAGE_MAX)
    {
        // over voltage protection
        // shut off pack and write OV error and cell id to eeprom
        return 0;
    }
    else if (g_lowest_voltage <= VOLTAGE_MIN)
    {
        // under voltage protection
        // shut off pack and write UV error and cell if to eeprom
        return 0;
    }
    else
    {
        // cell voltages are fine
        return 1;
    }
}

int1 check_temperature(void)
{
    // Find highest temperature reading
    ads7952_read_all_channels(g_adc_data);
    convert_adc_data_to_temps();
    g_highest_temperature_cell_index = get_highest_temperature_cell_index();
    g_highest_temperature = g_temps[g_highest_temperature_cell_index];
    
    if (g_highest_temperature >= TEMP_CRITICAL)
    {
        // temperature discharge protection
        // shut off pack, write OT error and cell id to eeprom
        return 0;
    }
    else if (g_highest_temperature >= TEMP_WARNING)
    {
        // tempeature charge protection
        // disable charging
        //if (motor controller and mppt are connected)
        if (true)
        {
            //turn off array, tell motor controller to disable regen
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        // cell temperatures are fine
        return 1;
    }
}

int1 check_current(void)
{
    // Read the pack current
    g_current = hall_sensor_read_data();
    
    // Hall effect sensor is uncalibrated
    // This function will always return true until calibration is done
    return 1;
    
    /*if (g_current >= CURRENT_DISCHARGE_LIMIT)
    {
        // discharge current protection
        // shut off pack, write OC error to eeprom
        return 0;
    }
    else if (g_current <= CURRENT_CHARGE_LIMIT)
    {
        // charge current protection
        // shut off pack, write UC error to eeprom
        return 0;
    }
    else
    {
        // current is fine
        return 1;
    }*/
}

// Main
void main()
{
    int i;
    
    // Kilovac is initially disabled
    KILOVAC_OFF;
    
    init_cells();
    ltc6804_init();
    ads7952_init();
    hall_sensor_init();
    lcd_init();
    fan_init();
    
    // Populate running average
    for (i = 0 ; i < N_SAMPLES ; i++)
    {
        ltc6804_read_cell_voltages(g_cell);
        average_voltage();
    }
    
    // Perform startup test
    if (SAFETY_CHECK == 1)
    {
        // Voltage, temperature, and current are all safe, connect the pack
        delay_ms(500);
        KILOVAC_ON;
        printf("SUCCESS");
        // Set up and enable timer 2 with a period of HEARTBEAT_PERIOD_MS
        setup_timer2(TMR_INTERNAL|TMR_DIV_BY_256,39*HEARTBEAT_PERIOD_MS);
        enable_interrupts(INT_TIMER2);
    }
    else
    {
        // Something went wrong, do not connect the pack
        KILOVAC_OFF;
    }
    
    while (true)
    {
        output_toggle(TX_LED);
        delay_ms(200);
        
        if (SAFETY_CHECK)
        {
            // Operating levels are safe, balance the cells
            balance();
            delay_ms(100);
        }
        else
        {
            // Something went wrong
            ltc6804_init(); // Disable balancing
            KILOVAC_OFF;    // Turn off pack
        }
        
        // Display data
        for (i = 0 ; i < 12 ; i++)
        {
            printf("\r\nVOLTAGE:\t%Lu",g_cell[i].average_voltage);
            ((g_discharge1>>i)&1) ? printf(" 1") : printf(" 0");
            if (i == g_highest_voltage_cell_index)
            {
                printf(" hi");
            }
            else if (i == g_lowest_voltage_cell_index)
            {
                printf(" lo");
            }
            else
            {
                printf("   ");
            }
            printf("\t\tTEMPERATURE:\t%Lu\t%f",g_adc_data[i],g_temps[i]);
        }
        for (i = 12 ; i < 24 ; i++)
        {
            printf("\r\nVOLTAGE:\t%Lu",g_cell[i].average_voltage);
            ((g_discharge2>>(i-12))&1) ? printf(" 1") : printf(" 0");
            if (i == g_highest_voltage_cell_index)
            {
                printf(" hi");
            }
            else if (i == g_lowest_voltage_cell_index)
            {
                printf(" lo");
            }
            else
            {
                printf("   ");
            }
            printf("\t\tTEMPERATURE:\t%Lu\t%f",g_adc_data[i],g_temps[i]);
        }
        for (i = 24 ; i < N_CELLS ; i++)
        {
            printf("\r\nVOLTAGE:\t%Lu",g_cell[i].voltage);
            ((g_discharge3>>(i-24))&1) ? printf(" 1") : printf(" 0");
            if (i == g_highest_voltage_cell_index)
            {
                printf(" hi");
            }
            else if (i == g_lowest_voltage_cell_index)
            {
                printf(" lo");
            }
            else
            {
                printf("   ");
            }
        }
        printf("\r\n\nCURRENT:\t%Lu",g_current);
    }
}

