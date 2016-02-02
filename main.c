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

static cell_t         g_cell[N_CELLS];
static unsigned int16 g_adc_data[N_ADC_CHANNELS];
static float          g_temps[N_ADC_CHANNELS];
static int            g_highest_voltage_cell_index;
static int            g_lowest_voltage_cell_index;

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

    printf("\n\n\n\n\n\n\rLower:\t%Lu\t%Lu\t%Lu\t%Lu",
           g_cell[0].average_voltage,
           g_cell[1].average_voltage,
           g_cell[2].average_voltage,
           g_cell[3].average_voltage);
    printf("\n\rUpper:\t%Lu\t%Lu\t%Lu\t%Lu",
           g_cell[12].average_voltage,
           g_cell[13].average_voltage,
           g_cell[14].average_voltage,
           g_cell[15].average_voltage);
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
        //ltc6804_read_cell_voltages(g_cell);
        
        output_low(CSBI1);
        ltc6804_write_command(ADCV);
        output_high(CSBI1);
        
        delay_us(500);
        
        output_low(CSBI1);
        ltc6804_write_command(RDCVA);
        data[0] = spi_read(0xFF);
        data[1] = spi_read(0xFF);
        data[2] = spi_read(0xFF);
        data[3] = spi_read(0xFF);
        data[4] = spi_read(0xFF);
        data[5] = spi_read(0xFF);
        data[6] = spi_read(0xFF);
        data[7] = spi_read(0xFF);
        output_high(CSBI1);
        printf("\n\n\n\n\n\n\rUpper:\t%X\t%X\t%X\t%X\t%X\t%X\t%X\t%X",
            data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
        
        
        output_low(CSBI2);
        ltc6804_write_command2(ADCV);
        output_high(CSBI2);
        
        delay_us(500);
        
        output_low(CSBI2);
        ltc6804_write_command2(RDCVA);
        data[0] = spi_read2(0xFF);
        data[1] = spi_read2(0xFF);
        data[2] = spi_read2(0xFF);
        data[3] = spi_read2(0xFF);
        data[4] = spi_read2(0xFF);
        data[5] = spi_read2(0xFF);
        data[6] = spi_read2(0xFF);
        data[7] = spi_read2(0xFF);
        output_high(CSBI2);
        printf("\n\rLower:\t%X\t%X\t%X\t%X\t%X\t%X\t%X\t%X",
            data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
            
        //print_cell_voltages();
        
        delay_ms(20);
    }
}

