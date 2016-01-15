#include <24HJ128GP502.h>

#device PASS_STRINGS = IN_RAM

#FUSES WPRES32                   //Watch Dog Timer PreScalar 1:32
#FUSES WPOSTS1                   //Watch Dog Timer PostScalar 1:1
#FUSES CKSFSM                    //Clock Switching is enabled, fail Safe clock monitor is enabled

// Use internal oscillator for delay
#use delay(internal = 20000000)

// UART port (PIC24HJ128GP502)
#use rs232(baud = 57600, xmit = PIN_B8, rcv = PIN_B9)

// SPI port 1: LTC6804-1
#pin_select SDI1     = PIN_B0  // MISO
#pin_select SDO1     = PIN_B1  // MOSI
#pin_select SCK1OUT  = PIN_B2  // SCK
#define     CSBI       PIN_B3  // LTC chip select, active low

// SPI port 2: ADS7952
#pin_select SDI2     = PIN_B15 // MISO
#pin_select SDO2     = PIN_B14 // MOSI
#pin_select SCK2OUT  = PIN_B13 // SCK
#define     ADC1_SEL   PIN_B12 // ADC-1, cells 1-12
#define     ADC2_SEL   PIN_B13 // ADC-2, cells 13-24

// Miscellaneous pins
#define HEARTBEAT_LED1 PIN_A4 // Hearbeat and diagnostic LED
#define HEARTBEAT_LED2 PIN_B5 // Hearbeat and diagnostic LED
#define BALANCE_PIN    PIN_B6 // Balancing button
#define UNBALANCE_PIN  PIN_B7 // Unbalancing button
