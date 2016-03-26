// BPMS Prototype code
// Authors: Andy Li, Stefan Dumitrescu, Mhamad Salih
// Copyright 2016, McMaster Solar Car Project
// Monitors voltage and performs charge balancing for 4 cells

// Includes
#include "main.h"
#include "math.h"
#include "pec.c"
#include "ltc6804.c"
#include "adc.c"

// Configuration
#fuses WPRES32 // Watch Dog Timer PreScalar 1:32
#fuses WPOSTS1 // Watch Dog Timer PostScalar 1:1
#fuses CKSFSM  // Clock Switching is enabled, fail Safe clock monitor is enabled

// PIC internal register addresses
#word IFS0 = 0x0084

// Macros to disable timers and clear flags
#define CLEAR_T2_FLAG IFS0  &= 0xFF7F

// Telemetry packet IDs
#define VOLTAGE_ID  0x5A
#define TEMP_ID     0x69
#define BALANCE_ID  0x41

static cell_t         g_cell[N_CELLS];
static unsigned int16 g_adc_data[N_ADC_CHANNELS];
static float          g_temps[N_ADC_CHANNELS];
static int            g_highest_voltage_cell_index;
static int            g_lowest_voltage_cell_index;

// Set up timer 2 as a millisecond timer
int16 g_ms;
#int_timer2 level = 4
void isr_timer2(void)
{
    g_ms++; //keep a running timer interupt that increments every milli-second
    CLEAR_T2_FLAG;
}

// Initializes the cells, clears all flags, resets highest and lowest cells
void init_cells(void)
{
    int i;
    
    for (i = 0 ; i < N_CELLS ; i++)
    {
        g_cell[i].voltage = 0;
        g_cell[i].temperature = 0;
        g_cell[i].ov_flag = 0;
        g_cell[i].uv_flag = 0;
        g_cell[i].ot_flag = 0;
    }
    
    g_cell[0].discharge = 0x00;
    g_cell[1].discharge = 0x02;
    g_cell[2].discharge = 0x04;
    g_cell[3].discharge = 0x08;
    
    g_highest_voltage_cell_index = 0;
    g_lowest_voltage_cell_index = 0;
}

// Returns the index for the highest voltage cell
int get_highest_voltage_cell_index(void)
{
    int i, highest = 0;
    for (i = 0 ; i < N_CELLS ; i++)
        if (g_cell[i].voltage >= g_cell[highest].voltage)
            highest = i;
    return highest;
}

// Returns the index for the lowest voltage cell
int get_lowest_voltage_cell_index(void)
{
    int i, lowest = 0;
    for (i = 0 ; i <= 3 ; i++)
        if (g_cell[i].average_voltage 
            <= g_cell[lowest].average_voltage)
            lowest = i;
    for (i = 12 ; i <= 15 ; i++)
        if (g_cell[i].average_voltage 
            <= g_cell[lowest].average_voltage)
            lowest = i;
    return lowest;
}

// Use the simplified Steinhart-Hart equation to approximate temperatures
void convert_adc_data_to_temps(void)
{
    int i;
    for (i = 0; i < N_ADC_CHANNELS; i++)
    {
        float resistance = THERMISTOR_SERIES * (float)g_adc_data[i] / 
           (LSBS_PER_VOLT * THERMISTOR_SUPPLY - (float)g_adc_data[i]);
        float temperature = resistance / THERMISTOR_NOMINAL;
        temperature = log(temperature);
        temperature /= B_COEFF;
        temperature += 1.0 / (TEMPERATURE_NOMINAL + 273.15);
        temperature = 1.0 / temperature;
        temperature -= 273.15;
        g_temps[i] = temperature;
    }
}

// Debug code, remove later
void print_discharge_bits(void)
{
    int i;
    int32 discharge = (g_discharge1&0x0FFF)|((int32)((g_discharge2&0x0FFF)<<12));
    for (i = 0; i < 16; i++)
    {
        printf("%c", (g_discharge1 >> i) & 1 ? '1' : '0');
    }
    printf("\r\n");
    for (i = 0; i < 16; i++)
    {
        printf("%c", (g_discharge2 >> i) & 1 ? '1' : '0');
    }
    for (i = 0; i < 32; i++)
    {
        printf("%c", (discharge >> i) & 1 ? '1' : '0');
    }
    printf("\r\n");
}

void print_temperatures(void)
{
    printf("temp[%d] = %d\t\t", 0, (int)(g_temps[0] * 10));
    printf("temp[%d] = %d\t\t", 1, (int)(g_temps[1] * 10));
    printf("temp[%d] = %d\r\n", 2, (int)(g_temps[2] * 10));
}

// Discharge all the cells that are 1% of the SoC range voltage higher than
// the lowest voltage
void balance(void)
{
    ltc6804_read_cell_voltages(g_cell);
    int min_idx = get_lowest_voltage_cell_index();
    int i;

    for (i = 0; i <= 3; i++)
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

    for (i = 12; i <= 15; i++)
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

    output_low(CSBI1);
    ltc6804_write_config(g_discharge1);
    output_high(CSBI1);

    output_low(CSBI2);
    ltc6804_write_config(g_discharge2);
    output_high(CSBI2);
}

void average_voltage(void)
{
    int i,j;
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
    int32 discharge = (g_discharge1&0x0FFF)|((int32)((g_discharge2&0x0FFF)<<12));
    putc(BALANCE_ID);
    putc((int8)(discharge&0xFF));
    putc((int8)((discharge>>8)&0xFF));
    putc((int8)((discharge>>16)&0xFF));
    putc((int8)((discharge>>24)&0xFF));
}

// Main
void main()
{
    int data[8];
    // Set up and enable timer 2 to interrupt every 1ms using 20MHz clock
    setup_timer2(TMR_INTERNAL|TMR_DIV_BY_256,39);
    enable_interrupts(INT_TIMER2);

    // Set up SPI ports
    setup_spi(SPI_MASTER|SPI_SCK_IDLE_HIGH|SPI_CLK_DIV_12|SPI_XMIT_L_TO_H);
    setup_spi2(SPI_MASTER|SPI_SCK_IDLE_LOW|SPI_CLK_DIV_12|SPI_XMIT_L_TO_H);
    
    init_PEC15_Table();
    init_cells();
    
    ltc6804_wakeup();
    ltc6804_init();
    //ads7952_init();

    while (true)
    {
        ltc6804_read_cell_voltages(g_cell);
        average_voltage();
        send_voltage_data();
        delay_ms(10);
        
        ads7952_read_all_channels(g_adc_data);
        convert_adc_data_to_temps();
        send_temperature_data();
        print_temperatures();
        delay_ms(10);
        
        balance();
        send_balancing_bits();
        delay_ms(200);
    }
}

