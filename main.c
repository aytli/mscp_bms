// BPMS Prototype code
// Authors: Andy Li, Stefan Dumitrescu, Mhamad Salih
// Copyright 2015, McMaster Solar Car Project
// Monitors voltage and performs charge balancing for 4 cells
// TODO: ADD FUNCTIONALITY FOR SECOND LTC CHIP USED ONLY FOR CHARGING/DISCHARGING
// TODO: FIX TIMER ISSUE

// Includes
#include <24HJ128GP502.h>
//#include "LTC6804-2_functions.h"

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

// Pins for STP08DP05 LED drivers
#define OE1 PIN_B6
#define OE2 PIN_B7

// NOTE:
// LTC-1 is an LTC6804 chip used to both poll and charge/discharge the cells
// LTC-2 is an LTC6804 chip used only to charge/discharge the cells
// LTC6804 datasheet: http://cds.linear.com/docs/en/datasheet/680412fb.pdf

// LTC6804 spi commands, 11 bits each
#define WRCFG   0x0001 // Write configuration register group
#define RDCFG   0x0002 // Read configuration register group
#define RDCVA   0x0004 // Read cell voltage register group A (cells 1-3)
#define RDCVB   0x0006 // Read cell voltage register group B (cells 4-6)
#define RDCVC   0x0008 // Read cell voltage register group C (cells 7-9)
#define RDCVD   0x000A // Read cell voltage register group D (cells 10-12)
#define RDAUXA  0x000C // Read auxiliary register group A
#define RDAUXB  0x000E // Read auxiliary register group B
#define RDSTATA 0x0010 // Read status register group A
#define RDSTATB 0x0012 // Read status register group B (contains voltage flags)
#define CLRCELL 0x0711 // Clear cell voltage register group
#define CLRAUX  0x0712 // Clear auxiliary register group
#define CLRSTAT 0x0713 // Clear status register group
#define PLADC   0x0714 // Poll ADC conversion status
#define DIAGN   0x0715 // Diagnose MUX and poll status
#define WRCOMM  0x0721 // Write COMM register group
#define RDCOMM  0x0722 // Read COMM register group
#define STCOMM  0x0723 // Start I2C/SPI communication
#define ADCV    0x0270 // Start cell voltage ADC conversion and poll status
                       // - MD = 01, 27kHz mode (fast) with ADCOPT = 0
                       // - DCP = 1, discharge permitted during ADC conversion
                       // - CHST = 000, 748us conversion time

// LTC6804 configuration bytes (bytes 4 and 5 used for charging/discharging)
#define CFGR0   0x0C   // VREFON = 1, ADCOPT = 0
#define CFGR1   0xD6   // Undervoltage = 2.80V (0x6D6)
#define CFGR2   0x06   // Overvoltage lower nibble + undervoltage upper nibble
#define CFGR3   0xA4   // Overvoltage  = 4.20V (0xA40)

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

// Number of channels on the LTC6804, and number of channels being used
#define N_CHANNELS 12 // The LTC6804 can monitor up to 12 cells
#define N_CELLS    4  // Number of cells actually connected

// Charge and discharge time
#define CHARGE_MS    5
#define DISCHARGE_MS 5

// CRC polynomial
#define CRC15_POLY   0x4599

// Struct for a cell
typedef struct
{
    unsigned int16 voltage; // LTC6804 has a 16 bit voltage ADC
    int8 temperature;
    int8 ov_flag;
    int8 uv_flag;
    int8 ot_flag;
} cell_t;

static cell_t g_cell[N_CHANNELS];
static int16  g_discharge_cell_mask;
static int16  g_charge_cell_mask;
static int1   gb_ready_to_balance;
static int    g_highest_voltage_cell_index;
static int    g_lowest_voltage_cell_index;

unsigned int16 pec15Table[256];

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

// Sends an 11 bit (2 bytes) command to LTC-1
void ltc6804_send_command(int16 command)
{
    spi_write((command&(0xFF00))>>8);
    spi_write(command&(0x00FF));
}

// Sends an 11 bit (2 bytes) command to LTC-2
void ltc6804_send_command2(int16 command)
{
    spi_write2((command&(0xFF00))>>8);
    spi_write2(command&(0x00FF));
}

// Sends two bytes of config data to LTC-1
void ltc6804_send_config(int16 data)
{
    ltc6804_send_command(WRCFG);
    spi_write(CFGR0);
    spi_write(CFGR1);
    spi_write(CFGR2);
    spi_write(CFGR3);
    spi_write((data&0xFF00)>>8);
    spi_write(data&0x00FF);
}

// Sends two bytes of config data to LTC-2
void ltc6804_send_config2(int16 data)
{
    ltc6804_send_command2(WRCFG);
    spi_write2(CFGR0);
    spi_write2(CFGR1);
    spi_write2(CFGR2);
    spi_write2(CFGR3);
    spi_write2((data&0xFF00)>>8);
    spi_write2(data&0x00FF);
}

// Sends configuration bytes to LTC-1 and LTC-2
void ltc6804_init(void)
{
    output_low(CSBI1);
    output_low(CSBI2);
    ltc6804_send_config(0x0000);
    ltc6804_send_config2(0x0000);
    output_high(CSBI1);
    output_high(CSBI2);
}

// Starts the cell voltage ADC conversion
void ltc6804_start_cell_voltage_adc_conversion(void)
{
    output_low(CSBI1);
    ltc6804_send_command(ADCV);
    ltc6804_send_command(0xAF42);
    output_high(CSBI1);
}

// Receives a pointer to an array of cells, writes the cell voltage to each one
void ltc6804_read_cell_voltages(cell_t * cell)
{
    int i,msb = 0x69,lsb = 0xFF;
    
    // Start the cell voltage adc conversion
    ltc6804_start_cell_voltage_adc_conversion();
    
    // Wait 1ms for the conversion to complete
    delay_ms(1);
    
    output_low(CSBI1);
    
    ltc6804_send_command(RDCVA); // voltage data for cells 1-3
    for (i = 0 ; i < 3 ; i ++)
    {
        //lsb = spi_read();
        //msb = spi_read();
        cell[i].voltage = (msb << 8) + lsb;
    }
    
    ltc6804_send_command(RDCVB); // voltage data for cells 4-6
    for (i = 3 ; i < 6 ; i ++)
    {
        //lsb = spi_read();
        //msb = spi_read();
        cell[i].voltage = (msb << 8) + lsb;
    }
    
    // Prototype only contains 4 cells, no need to measure cells 7-12
    /*ltc6804_send_command(RDCVC); // voltage data for cells 7-9
    for (i = 6 ; i < 9 ; i ++)
    {
        lsb = spi_read();
        msb = spi_read();
        cell[i]->voltage = (msb << 8) + lsb;
    }
    
    ltc6804_send_command(RDCVD); // voltage data for cells 10-12
    for (i = 9 ; i < 12 ; i ++)
    {
        lsb = spi_read();
        msb = spi_read();
        cell[i]->voltage = (msb << 8) + lsb;
    }*/
    
    output_high(CSBI1);
}

// Receives a pointer to an array of cells, sets ov and uv flags for each one
void ltc6804_read_voltage_flags(cell_t * cell)
{
    int i,data;

    output_low(CSBI1);
    
    ltc6804_send_command(RDSTATB); // voltage flags are stored in register group B
    data = spi_read(); // 1st byte = digital power supply voltage lsb, useless
    data = spi_read(); // 2nd byte = digital power supply voltage msb, useless
    
    data = spi_read(); // 3rd byte = ov and uv flags for cells 1-4
    for (i = 0 ; i < 3 ; i+=2)
    {
        cell[i].uv_flag = (data & (1 <<  i   )) >>  i;
        cell[i].ov_flag = (data & (1 << (i+1))) >> (i+1);
    }
    
    data = spi_read(); // 4th byte = ov and uv flags for cells 5-8
    for (i = 4 ; i < 7 ; i+=2)
    {
        cell[i].uv_flag = (data & (1 <<  i   )) >>  i;
        cell[i].ov_flag = (data & (1 << (i+1))) >> (i+1);
    }
    
    data = spi_read(); // 5th byte = ov and uv flags for cells 9-12
    for (i = 8 ; i < 11 ; i+=2)
    {
        cell[i].uv_flag = (data & (1 <<  i   )) >>  i;
        cell[i].ov_flag = (data & (1 << (i+1))) >> (i+1);
    }
    
    data = spi_read(); // 6th byte = revision code and other useless bits
    
    output_high(CSBI1);
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

// Write discharge bits, start discharge timer
void start_discharge(void)
{
    gb_ready_to_balance = false;
    
    // Pull CSBI low
    output_low(CSBI1);
    output_low(CSBI2);
    
    ltc6804_send_config (g_discharge_cell_mask);
    ltc6804_send_config2(g_discharge_cell_mask);
    
    // Pull CSBI high to start discharging
    output_high(CSBI1);
    output_high(CSBI2);
    
    // Pull CSBI low
    output_low(CSBI1);
    output_low(CSBI2);
    
    // Write charge bits
    ltc6804_send_config (g_charge_cell_mask);
    ltc6804_send_config2(g_charge_cell_mask);
    
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
    output_high(CSBI2);
    
    // Pull CSBI low
    output_low(CSBI1);
    output_low(CSBI2);
    
    // Clear charge bits
    spi_write((WRCFG&(0xF0))>>8);
    spi_write(WRCFG&(0x0F));
    spi_write(CFGR0);
    spi_write(CFGR1);
    spi_write(CFGR2);
    spi_write(CFGR3);
    spi_write(0x00);
    spi_write(0x00);
    
    spi_write2((WRCFG&(0xF0))>>8);
    spi_write2(WRCFG&(0x0F));
    spi_write2(CFGR0);
    spi_write2(CFGR1);
    spi_write2(CFGR2);
    spi_write2(CFGR3);
    spi_write2(0x00);
    spi_write2(0x00);
    
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
    output_high(CSBI2);
    
    gb_ready_to_balance = true;
    
    // Disable charge timer
    disable_interrupts(INT_TIMER4);
    DISABLE_T4;
    CLEAR_T4_FLAG;
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

void init_PEC15_Table()
{
    int i;
    int bit;
    unsigned int16 remainder;
    for (i = 0; i < 256; i++)
    {
        remainder = i << 7;
        for (bit = 8; bit > 0; --bit)
        {
            if (remainder & 0x4000)
            {
                remainder = ((remainder << 1));
                remainder = (remainder ^ CRC15_POLY);
            }
            else
            {
                remainder = ((remainder << 1));
            }
        }
        pec15Table[i] = remainder & 0xFFFF;
    }
}

unsigned int16 pec15(char *data , int len)
{
    unsigned int16 remainder, address;
    remainder = 16;
    int i;
    for (i = 0; i < len; i++) {
        address = ((remainder >> 7) ^ data[i]) & 0xff;
        remainder = (remainder << 8 ) ^ pec15Table[address];
    }
    return (remainder * 2);
}

// Main
void main()
{
    int data;
    int i;
    
    // Set up and enable timer 2 to interrupt every 1ms using 20MHz clock
    setup_timer2(TMR_INTERNAL|TMR_DIV_BY_256,39);
    enable_interrupts(INT_TIMER2);

    // Set up two SPI ports
    setup_spi (SPI_MASTER|SPI_SCK_IDLE_HIGH|SPI_XMIT_L_TO_H|SPI_CLK_DIV_12);
    
    output_low(CSBI1);
    delay_us(1);
    output_high(CSBI1);
    delay_us(10);
    output_low(CSBI1);
    spi_write(0x80);
    spi_write(0x01);
    spi_write(0x4D);
    spi_write(0x7A);
    spi_write(0xE5);
    spi_write(0xE2);
    spi_write(0x44);
    spi_write(0x9C);
    spi_write(0x00);
    spi_write(0x00);
    spi_write(0xF1);
    spi_write(0x66);
    output_high(CSBI1);
    
    delay_ms(5);
    
    output_low(CSBI1);
    delay_us(1);
    output_high(CSBI1);
    delay_us(10);
    output_low(CSBI1);
    spi_write(0x87);
    spi_write(0x11);
    spi_write(0xB9);
    spi_write(0xD4);
    output_high(CSBI1);
    
    delay_ms(2);

    output_low(CSBI1);
    delay_us(1);
    output_high(CSBI1);
    delay_us(10);
    output_low(CSBI1);
    spi_write(0x83);
    spi_write(0x70);
    spi_write(0xDF);
    spi_write(0x56);
    output_high(CSBI1);
    
    delay_ms(3);
    
    output_low(CSBI1);
    delay_us(1);
    output_high(CSBI1);
    delay_us(10);
    output_low(CSBI1);
    spi_write(0x80);
    spi_write(0x04);
    spi_write(0x77);
    spi_write(0xD5);
    spi_read(0x00);
    spi_read(0x00);
    spi_read(0x00);
    spi_read(0x00);
    spi_read(0x00);
    spi_read(0x00);
    spi_read(0x00);
    spi_read(0x00);
    output_high(CSBI1);
    
    while(true)
    {
        if (ms >= 200)
        {
            ms = 0;
            output_toggle(HEARTBEAT_LED);
        }
    }
}

