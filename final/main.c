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

// Kilovac control
#define KILOVAC_ON  output_high(KVAC_PIN);
#define KILOVAC_OFF output_low(KVAC_PIN);

// Status LED blink period
#define HEARTBEAT_PERIOD_MS 500

static cell_t         g_cell[N_CELLS];
static unsigned int16 g_adc_data[N_ADC_CHANNELS];
static float          g_temps[N_ADC_CHANNELS];
static int            g_highest_voltage_cell_index;
static int            g_lowest_voltage_cell_index;

// Timer 2 is used to send LabVIEW data, and to perform charge balancing
#int_timer2 level = 4
void isr_timer2(void)
{
    // Send data to LabVIEW over uart
    /*send_voltage_data();
    delay_ms(10);
    send_temperature_data();
    delay_ms(10);
    balance();
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

// Main
void main()
{
    // Set up and enable timer 2 with a period of HEARTBEAT_PERIOD_MS
    setup_timer2(TMR_INTERNAL|TMR_DIV_BY_256,39*HEARTBEAT_PERIOD_MS);
    enable_interrupts(INT_TIMER2);

    // Set up SPI ports
    setup_spi(SPI_MASTER|SPI_SCK_IDLE_HIGH|SPI_CLK_DIV_12|SPI_XMIT_L_TO_H);
    setup_spi2(SPI_MASTER|SPI_SCK_IDLE_LOW|SPI_CLK_DIV_12|SPI_XMIT_L_TO_H);
    
    init_cells();
    ltc6804_init();
    ads7952_init();
    hall_sensor_init();
    fan_init();
    
    // Connect the battery pack
    KILOVAC_ON;
    
    lcd_init();
    
    int8 error;
    int8 id;
    
    while (true)
    {
        eeprom_write_error(0x04,0x69);
        error = (int8)(eeprom_read_error());
        id = (int8)(eeprom_read_id());
        
        //itoa(error,16,error_str);
        //itoa(id,16,id_str);
        
        //lcd_set_cursor_position(0,0);
        //lcd_write("ERROR: ");
        //lcd_write(error_str);
        //lcd_set_cursor_position(1,0);
        //lcd_write("ID: ");
        //lcd_write(id_str);
        
        output_toggle(TX_LED);
        delay_ms(100);
        
        // PROTECTION CODE
        /*
        // Find highest and lowest cell voltages
        ltc6804_read_cell_voltages(g_cell);
        average_voltage();
        g_highest_voltage_cell_index = get_highest_voltage_cell_index();
        g_lowest_voltage_cell_index = get_lowest_voltage_cell_index();
        highest_voltage = g_cell[g_highest_voltage_cell_index].average_voltage;
        lowest_voltage = g_cell[g_lowest_voltage_cell_index].average_voltage;
        
        if (highest_voltage >= VOLTAGE_MAX)
        {
            // over voltage protection
            // shut off pack and write OV error and cell id to eeprom
            KILOVAC_OFF;
        }
        else if (lowest_voltage <= VOLTAGE_MIN)
        {
            // under voltage protection
            // shut off pack and write UV error and cell if to eeprom
            KILOVAC_OFF;
        }
        else
        {
            // cell voltages are fine, do nothing
        }
        
        // Find highest temperature reading
        ads7952_read_all_channels(g_adc_data);
        convert_adc_data_to_temps();
        g_highest_temperature_cell_index = get_highest_temperature_cell_index();
        highest_temperature = g_adc_data[g_highest_temperature_cell_index];
        
        if (highest_temperature >= TEMP_CRITICAL)
        {
            // temperature discharge protection
            // shut off pack, write OT error and cell id to eeprom
            KILOVAC_OFF;
        }
        else if (highest_temperature >= TEMP_WARNING)
        {
            // tempeature charge protection
            // disable charging
            if (motor controller and mppt are connected)
            {
                turn off array, tell motor controller to disable regen
            }
            else
            {
                KILOVAC_OFF;
            }
        }
        else
        {
            // cell temperatures are fine, do nothing
        }
        
        // Read the pack current
        g_current = hall_sensor_read_data();
        
        if (g_current >= CURRENT_DISCHARGE_LIMIT)
        {
            // discharge current protection
            // shut off pack, write OC error to eeprom
            KILOVAC_OFF;
        }
        else if (g_current <= CURRENT_CHARGE_LIMIT)
        {
            // charge current protection
            // shut off pack, write UC error to eeprom
            KILOVAC_OFF;
        }
        else
        {
            // current is fine, do nothing
        }*/
    }
}

