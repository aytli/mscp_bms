// BPMS Prototype code
// Authors: Andy Li, Stefan Dumitrescu, Mhamad Salih
// Copyright 2015, McMaster Solar Car Project
// Monitors voltage and performs charge balancing for 4 cells
// TODO: ADD FUNCTIONALITY FOR SECOND LTC CHIP USED ONLY FOR CHARGING/DISCHARGING
// TODO: FIX TIMER ISSUE

// Includes
#include <24HJ128GP502.h>
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

// Use internal oscillator for delay
#use delay(internal = 20000000)

// UART port (PIC24HJ128GP502)
#use rs232(baud = 9600, xmit = PIN_B8, rcv = PIN_B9)

// Miscellaneous pins
#define HEARTBEAT_LED PIN_B5 // Hearbeat and diagnostic LED

// LTC6804 pins
#define CSBI1 PIN_B3  // LTC-1 chip select, active low
#define CSBI2 PIN_B12 // LTC-2 chip select, active low

// Setup SPI ports (PIC24HJ128GP502)
#pin_select SDI1    = PIN_B0 // MISO
#pin_select SDO1    = PIN_B1 // MOSI
#pin_select SCK1OUT = PIN_B2 // SCK
#pin_select SS1OUT  = CSBI1  // SS

/*#pin_select SDI2    = PIN_B15
#pin_select SDO2    = PIN_B14
#pin_select SCK2OUT = PIN_B13
#pin_select SS2OUT  = CSBI2*/

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

// Set up timer 2 as a millisecond timer
int16 ms;
#int_timer2 level = 4
void isr_timer2(void)
{
    ms++; //keep a running timer interupt that increments every milli-second
    CLEAR_T2_FLAG;
}

/*int16 calculate_pec(int64 data, int n_bytes)
{
    int1 PEC[15] = {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0};
    int1 DIN,IN0,IN3,IN4,IN7,IN8,IN10,IN14;
    int i;
    
    for (i = n_bytes*8-1; i >= 0 ; i--)
    {
        DIN  = (data&(1<<i))>>i;
        IN0  = DIN^PEC[14];
        IN3  = IN0^PEC[2];
        IN4  = IN0^PEC[3];
        IN7  = IN0^PEC[6];
        IN8  = IN0^PEC[7];
        IN10 = IN0^PEC[9];
        IN14 = IN0^PEC[13];
        
        PEC[14] = IN14;
        PEC[13] = PEC[12];
        PEC[12] = PEC[11];
        PEC[11] = PEC[10];
        PEC[10] = IN10;
        PEC[9]  = PEC[8];
        PEC[8]  = IN8;
        PEC[7]  = IN7;
        PEC[6]  = PEC[5];
        PEC[5]  = PEC[4];
        PEC[4]  = IN4;
        PEC[3]  = IN3;
        PEC[2]  = PEC[1];
        PEC[1]  = PEC[0];
        PEC[0]  = IN0;
    }
    
    return (PEC[14]*BIT15)|(PEC[13]*BIT14)|(PEC[12]*BIT13)|
           (PEC[11]*BIT12)|(PEC[10]*BIT11)|(PEC[9]*BIT10)|
           (PEC[8]*BIT9)  |(PEC[7]*BIT8)  |(PEC[6]*BIT7)|
           (PEC[5]*BIT6)  |(PEC[4]*BIT5)  |(PEC[3]*BIT4)|
           (PEC[2]*BIT3)  |(PEC[1]*BIT2)  |(PEC[0]*BIT1);
}*/

void LTC_wakeup()
{
    //Wake up serial interface
    output_low(CSBI1);
    delay_us(2);
    output_high(CSBI1);
    delay_us(14);
    output_low(CSBI1);
}

void LTC_refon_set()
{
    spi_write(0x80);
    spi_write(0x01);
    spi_write(0x4D);
    spi_write(0x7A);
    delay_us(2);
    
    spi_write(0xE5);
    spi_write(0xE2);
    spi_write(0x44);
    spi_write(0x9C);
    delay_us(2);
    spi_write(0xFF);
    spi_write(0xFF);
    
    // Calculate PEC
    //unsigned int16 byte_CRC16[6] = {0xE5, 0x15, 0x57, 0x98, 0x00, 0x00};
    //unsigned int16 PEC_A = (LTC_pec_calc(byte_CRC16, 6)) & 0xFF00;
    //unsigned int16 PEC_B = (LTC_pec_calc(byte_CRC16, 6)) & 0x00FF;
    //printf("%x", PEC_A);
    //printf("%x", PEC_A);
    
    //spi_write(PEC_A);
    //spi_write(PEC_B);
    
    spi_write(0xF1);
    spi_write(0x66);
    
    
    //time delay for ADC to warm up and turn on.
    delay_us(100);
    output_high(CSBI1);
    delay_us(3900);
    //Can immediately run next function here with LTC wake up.
}

void LTC_ADC_clear()
{
    spi_write(0x87);
    spi_write(0x11);
    spi_write(0xB9);
    spi_write(0xD4);
    
    delay_us(20);
}

void LTC_ADC_conversion()
{
    spi_write(0x83);
    spi_write(0x70);
    spi_write(0xDF);
    spi_write(0x56);
    
    delay_us(20);
    output_high(CSBI1);
    delay_us(6000);
}

void LTC_read_voltages_123()
{
    spi_write(0x80);
    spi_write(0x04);
    spi_write(0x77);
    spi_write(0xD6);
    
    spi_write(0xFF);
    spi_write(0xFF);
    delay_us(2);
    spi_write(0xFF);
    spi_write(0xFF);
    spi_write(0xFF);
    spi_write(0xFF);
    delay_us(2);
    spi_write(0xFF);
    spi_write(0xFF);
    
    delay_us(20);
}

// Write discharge bits, start discharge timer
void start_discharge(int16 discharge, int16 charge)
{
    gb_ready_to_balance = false;
    LTC_wakeup();
    
    // Pull CSBI low, write discharge bits, pull CSBI high to start discharging
    output_low(CSBI1);
    ltc6804_send_config(discharge);
    output_high(CSBI1);
    
    // Pull CSBI low, write charge bits
    output_low(CSBI1);
    ltc6804_send_config(charge);
    
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
    ltc6804_send_config(0x00);
    
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

// Main
void main()
{
    char bytes[6];
    unsigned int16 crc = 0x0000;
    
    // Set up and enable timer 2 to interrupt every 1ms using 20MHz clock
    setup_timer2(TMR_INTERNAL|TMR_DIV_BY_256,39);
    enable_interrupts(INT_TIMER2);

    // Set up two SPI ports
    setup_spi(SPI_MASTER|SPI_SCK_IDLE_HIGH|SPI_CLK_DIV_2|SPI_CLK_DIV_5|SPI_XMIT_L_TO_H);
    
    init_PEC15_Table();
    
    output_high(CSBI1);
    DELAY_US(1000000);
    LTC_wakeup();
    
    // Send ADCV Command
    LTC_wakeup();
    
    output_low(CSBI1);
    spi_write(0x03);
    spi_write(0x60);
    spi_write(0xF4);
    spi_write(0x64);
    output_high(CSBI1);
    
    while (true)
    {
        start_discharge(BIT0, BIT1);
        delay_ms(5);
        start_discharge(BIT1, BIT2);
        delay_ms(5);
        start_discharge(BIT2, BIT3);
        delay_ms(5);
        start_discharge(BIT3, BIT0);        
        delay_ms(5);
    }
}
