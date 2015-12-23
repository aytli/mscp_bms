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
#define HEARTBEAT_LED1 PIN_A4 // Hearbeat and diagnostic LED
#define HEARTBEAT_LED2 PIN_B5 // Hearbeat and diagnostic LED
#define BALANCE_PIN    PIN_B6 // Balancing button
#define UNBALANCE_PIN  PIN_B7 // Unbalancing button


// Setup SPI ports (PIC24HJ128GP502)
#pin_select SDI1    = PIN_B0 // MISO
#pin_select SDO1    = PIN_B1 // MOSI
#pin_select SCK1OUT = PIN_B2 // SCK
#pin_select SS1OUT  = CSBI1  // SS

#pin_select SDI2    = PIN_B15
#pin_select SDO2    = PIN_B14
#pin_select SCK2OUT = PIN_B13
#pin_select SS2OUT  = CSBI2

// Discharge and charge time in us (cannot exceed 6553us)
#define DISCHARGE_US 500
#define CHARGE_US    2000

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
    
    g_cell[0].discharge1 = 0x00;
    g_cell[0].discharge2 = 0x00;
    g_cell[0].charge1 = 0x00;
    g_cell[0].charge2 = 0x00;
    
    g_cell[1].discharge1 = 0x02;
    g_cell[1].discharge2 = 0x01;
    g_cell[1].charge1 = 0x01;
    g_cell[1].charge2 = 0x02;
    
    g_cell[2].discharge1 = 0x04;
    g_cell[2].discharge2 = 0x02;
    g_cell[2].charge1 = 0x02;
    g_cell[2].charge2 = 0x04;
    
    g_cell[3].discharge1 = 0x08;
    g_cell[3].discharge2 = 0x04;
    g_cell[3].charge1 = 0x04;
    g_cell[3].charge2 = 0x08;
    
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

// Write discharge bits, start discharge timer
void start_balance(cell_t discharge, cell_t charge)
{
    gb_ready_to_balance = false;
    ltc6804_wakeup();
    ltc6804_wakeup2();
    
    // Pull CSBI low, write discharge bits, pull CSBI high to start discharging
    output_low(CSBI1);
    output_low(CSBI2);
    ltc6804_write_config(discharge.discharge1);
    ltc6804_write_config2(discharge.discharge2);
    output_high(CSBI1);
    output_high(CSBI2);
    
    // Pull CSBI low, write charge bits
    output_low(CSBI1);
    output_low(CSBI2);
    ltc6804_write_config(charge.charge1);
    ltc6804_write_config2(charge.charge2);
    
    // Enable discharge timer
    clear_interrupt(INT_TIMER3);
    setup_timer3(TMR_INTERNAL|TMR_DIV_BY_1,10*DISCHARGE_US);
    enable_interrupts(INT_TIMER3);
}

// Timer 3 is the discharge timer, stops discharge cycle, starts charge cycle
#int_timer3 level = 5
void isr_timer3(void)
{
    // Pull CSBI high to start charging
    output_high(CSBI1);
    output_high(CSBI2);
    
    // Pull CSBI low, write stop bits
    output_low(CSBI1);
    output_low(CSBI2);
    ltc6804_write_config(0x00);
    ltc6804_write_config2(0x00);
    
    // Disable discharge timer
    disable_interrupts(INT_TIMER3);
    DISABLE_T3;
    CLEAR_T3_FLAG;
    
    // Enable charge timer
    clear_interrupt(INT_TIMER4);
    setup_timer4(TMR_INTERNAL|TMR_DIV_BY_1,10*CHARGE_US);
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
    cell_t charge_cell, discharge_cell;
    
    // Set up and enable timer 2 to interrupt every 1ms using 20MHz clock
    setup_timer2(TMR_INTERNAL|TMR_DIV_BY_256,39);
    enable_interrupts(INT_TIMER2);

    // Set up SPI ports
    setup_spi(SPI_MASTER|SPI_SCK_IDLE_HIGH|SPI_CLK_DIV_12|SPI_XMIT_L_TO_H);
    setup_spi2(SPI_MASTER|SPI_SCK_IDLE_HIGH|SPI_CLK_DIV_12|SPI_XMIT_L_TO_H);
    
    init_PEC15_Table();
    init_cells();
    
    // Send ADCV Command
    ltc6804_wakeup();
    ltc6804_init();
    ltc6804_wakeup2();
    ltc6804_init2();
    
    while (true)
    {
        charge_cell = g_cell[2];
        discharge_cell = g_cell[1];
        
        if (input(UNBALANCE_PIN) == 1)
        {
            /* NOTE:
              
              The large delay value of (120+120)+(140+120+120) is roughly how
              long it will take to send one write config command to all 6 LTC
              chips on the two SPI ports. It takes about 120us to send 6 bytes
              of data + 2 bytes of PEC, and about 20 us for the command bytes.
              
              In its current state, the code only talks to LTC-1, and it will
              delay for a period of time to simulate commands being sent to the
              other chips on the daisy chain. It is set to control S1 and S2.
              
              This code will send 0x0001 to activate S1, delay for 5ms, then
              send 0x0002 to turn on S2, delay for 5ms, and then send 0x0000 to
              turn everything off. This code will run when the UNBALANCE button
              is pressed, this button is located on PIN_B7.
              
              The big problem we saw was that the S pins would turn on much
              faster than they turn off. This will cause the two external PMOSes
              to short for a period of about 80us.
              
              Putting a 22nF capacitor from the gate to ground (the negative
              terminal of that particular cell) will slow down the PMOS turn on
              time so that the overlap will be smaller, but it will also delay
              the turn off time. We tried finding a way to connect it so that
              the capacitor will charge and slow down the turn on time, and
              discharge when the PMOS is turned off.
              
              I wrote a lot of stuff here. Moe was there with me so he should
              understand this. If not, feel free to message me.
              
                  - Andy
            */
            
            output_toggle(HEARTBEAT_LED1);
            output_low(CSBI1);
            ltc6804_write_config(0x0001);
            delay_us((120+120) + (140+120+120));
            output_high(CSBI1);
            
            delay_ms(5);
            
            // This code will send a commmand to disable S1 before enabling S2.
            /*output_low(CSBI1);
            ltc6804_write_config(0x0000);
            delay_us((120+120) + (140+120+120));
            output_high(CSBI1);*/
            
            output_low(CSBI1);
            ltc6804_write_config(0x0002);
            delay_us((120+120) + (140+120+120));
            output_high(CSBI1);
            
            delay_ms(5);
            
            output_low(CSBI1);
            ltc6804_write_config(0x0000);
            delay_us((120+120) + (140+120+120));
            output_high(CSBI1);
            
            delay_ms(5);
            
            
            // OLD CODE FROM BUTTON FUNCTIONALITY
            //start_balance(charge_cell, discharge_cell);
            //while(gb_ready_to_balance == false);
            //ltc6804_read_cell_voltages(g_cell);
            //print_cell_voltages();
            //delay_ms(5);
        }
        /*else if (input(BALANCE_PIN) == 1)
        {
            output_toggle(HEARTBEAT_LED2);
            start_balance(discharge_cell, charge_cell);
            while(gb_ready_to_balance == false);
            ltc6804_read_cell_voltages(g_cell);
            print_cell_voltages();
            delay_ms(20);
        }*/
        else
        {
            output_low(HEARTBEAT_LED1);
            output_low(HEARTBEAT_LED2);
        }
    }
}
