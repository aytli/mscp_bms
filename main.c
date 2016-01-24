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
#word IFS0  = 0x0084

// Macros to disable timers and clear flags
#define CLEAR_T2_FLAG IFS0  &= 0xFF7F

static cell_t         g_cell[N_CHANNELS];
static unsigned int16 g_adc_data[N_ADC_CHANNELS];
static float          g_temps[N_ADC_CHANNELS];
static int            g_highest_voltage_cell_index;
static int            g_lowest_voltage_cell_index;

// Initializes the cells, clears all flags, resets highest and lowest cells
void init_cells(void)
{
    int i;
    
    for (i = 0 ; i < N_CHANNELS ; i++)
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
    for (i = 0 ; i < N_CELLS ; i++)
        if (g_cell[i].voltage <= g_cell[lowest].voltage)
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

void print_temperatures(void)
{
   int i;
   for (i = 0; i < N_ADC_CHANNELS; i++)
   {
      printf("temp[%d] = %f\r\n", i, g_temps[i]);
   }
   printf("\n");
}

void print_cell_voltages(void)
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
    
    printf("\n\n\n\n\n\n\rCell1: %Lu\tCell2: %Lu\tCell3: %Lu\tCell4: %Lu",
           g_cell[0].average_voltage,
           g_cell[1].average_voltage,
           g_cell[2].average_voltage,
           g_cell[3].average_voltage);
}

// Set up timer 2 as a millisecond timer
int16 g_ms;
#int_timer2 level = 4
void isr_timer2(void)
{
    g_ms++; //keep a running timer interupt that increments every milli-second
    CLEAR_T2_FLAG;
}

// Main
void main()
{
    // Set up and enable timer 2 to interrupt every 1ms using 20MHz clock
    setup_timer2(TMR_INTERNAL|TMR_DIV_BY_256,39);
    enable_interrupts(INT_TIMER2);

    // Set up SPI ports
    setup_spi(SPI_MASTER|SPI_SCK_IDLE_HIGH|SPI_CLK_DIV_12|SPI_XMIT_L_TO_H);
    setup_spi2(SPI_MASTER|SPI_SCK_IDLE_LOW|SPI_CLK_DIV_12|SPI_XMIT_L_TO_H);
    
    init_PEC15_Table();
    init_cells();
    
    // Send ADCV Command
    ltc6804_wakeup();
    ltc6804_init();
    
    ads7952_init();
    
    while (true)
    {
      if (g_ms >= 1000) 
      {                  
         g_ms = 0;
         ads7952_read_all_channels(g_adc_data);
         convert_adc_data_to_temps();
         
         print_temperatures();
      }
    }
}

