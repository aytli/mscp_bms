// BPMS Prototype code
// Authors: Andy Li, Stefan Dumitrescu, Mhamad Salih
// Copyright 2015, McMaster Solar Car Project
// Monitors voltage and performs charge balancing for 4 cells

// Includes
#include "main.h"
#include "pec.c"
#include "ltc6804.c"

// Configuration
#fuses WPRES32 // Watch Dog Timer PreScalar 1:32
#fuses WPOSTS1 // Watch Dog Timer PostScalar 1:1
#fuses CKSFSM  // Clock Switching is enabled, fail Safe clock monitor is enabled

// PIC internal register addresses
#word IFS0  = 0x0084
#word IFS1  = 0x0086
#word T2CON = 0x0110
#word T3CON = 0x0112
#word T4CON = 0x011E

// Bit masks
#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000

// Macros to disable timers and clear flags
#define DISABLE_T3    T3CON &= 0x7FFF
#define DISABLE_T4    T4CON &= 0x7FFF
#define CLEAR_T2_FLAG IFS0  &= 0xFF7F
#define CLEAR_T3_FLAG IFS0  &= 0xFEFF
#define CLEAR_T4_FLAG IFS1  &= 0xF7FF

// Miscellaneous pins
#define HEARTBEAT_LED PIN_B5 // Hearbeat and diagnostic LED

// Setup SPI ports (PIC24HJ128GP502)
#pin_select SDI1    = PIN_B0 // MISO
#pin_select SDO1    = PIN_B1 // MOSI
#pin_select SCK1OUT = PIN_B2 // SCK
#pin_select SS1OUT  = CSBI1  // SS

#pin_select SDI2    = PIN_B15
#pin_select SDO2    = PIN_B14
#pin_select SCK2OUT = PIN_B13
#pin_select SS2OUT  = CSBI2

// Charge and discharge time
#define CHARGE_MS    1
#define DISCHARGE_MS 1

static cell_t g_cell[N_CHANNELS];
static int1   gb_ready_to_balance;
static int    g_highest_voltage_cell_index;
static int    g_lowest_voltage_cell_index;

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

// Write discharge bits, start discharge timer
void start_discharge(int16 discharge, int16 charge)
{
    gb_ready_to_balance = false;
    ltc6804_wakeup();
    
    // Pull CSBI low, write discharge bits, pull CSBI high to start discharging
    output_low(CSBI1);
    ltc6804_write_config(discharge);
    output_high(CSBI1);
    
    // Pull CSBI low, write charge bits
    output_low(CSBI1);
    ltc6804_write_config(charge);
    
    // Enable discharge timer
    clear_interrupt(INT_TIMER3);
    setup_timer3(TMR_INTERNAL|TMR_DIV_BY_8,1250*DISCHARGE_MS);
    enable_interrupts(INT_TIMER3);
}

// Timer 3 is the discharge timer, stops discharge cycle, starts charge cycle
#int_timer3 level = 5
void isr_timer3(void)
{
    // Pull CSBI high to start charging
    output_high(CSBI1);
    
    // Pull CSBI low, write stop bits
    output_low(CSBI1);
    ltc6804_write_config(0x00);
    
    // Disable discharge timer
    disable_interrupts(INT_TIMER3);
    DISABLE_T3;
    CLEAR_T3_FLAG;
    
    // Enable charge timer
    clear_interrupt(INT_TIMER4);
    setup_timer4(TMR_INTERNAL|TMR_DIV_BY_8,1250*CHARGE_MS);
    enable_interrupts(INT_TIMER4);
}

// Timer 4 is the charge timer, stop charge cycle
#int_timer4 level = 5
void isr_timer4(void)
{
    // Pull CSBI high to stop charging
    output_high(CSBI1);
    
    gb_ready_to_balance = true;
    
    // Disable charge timer
    disable_interrupts(INT_TIMER4);
    DISABLE_T4;
    CLEAR_T4_FLAG;
}

// Set up timer 2 as a millisecond timer
int16 ms;
#int_timer2 level = 4
void isr_timer2(void)
{
    ms++; //keep a running timer interupt that increments every milli-second
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
    setup_spi2(SPI_MASTER|SPI_SCK_IDLE_HIGH|SPI_CLK_DIV_12|SPI_XMIT_L_TO_H);
    
    init_PEC15_Table();
    
    // Send ADCV Command
    ltc6804_wakeup();
    ltc6804_init();
    ltc6804_wakeup2();
    ltc6804_init2();
    
    while (true)
    {
        //start_discharge(BIT0,BIT1);
        ltc6804_wakeup();
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
        
        printf("\r\n%X\t%X\t%X\t%X\t%X\t%X\t%X\t%X",data[1],data[0],data[3],data[2],data[5],data[4],data[7],data[6]);
        
        ltc6804_wakeup2();
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
        
        printf("\r\n%X\t%X\t%X\t%X\t%X\t%X\t%X\t%X\n",data[1],data[0],data[3],data[2],data[5],data[4],data[7],data[6]);
        
        delay_ms(10);
    }
}
