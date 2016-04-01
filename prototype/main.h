#include <24HJ128GP502.h>

#device PASS_STRINGS = IN_RAM
#device ADC = 12

#fuses WPRES32 //Watch Dog Timer PreScalar 1:32
#fuses WPOSTS1 //Watch Dog Timer PostScalar 1:1
#fuses CKSFSM  //Clock Switching is enabled, fail Safe clock monitor is enabled

// Use internal oscillator for delay
#use delay(internal = 20000000)

// UART port (PIC24HJ128GP502)
#use rs232(baud = 57600, xmit = PIN_B8, rcv = PIN_B9)

// SPI port 1: LTC6804-1
#pin_select SDI1     = PIN_B0  // MISO
#pin_select SDO1     = PIN_B1  // MOSI
#pin_select SCK1OUT  = PIN_B2  // SCK
#define     CSBI1      PIN_B3  // LTC-1 chip select, active low
#define     CSBI2      PIN_A2  // LTC-2 chip select, active low
#define     MOSI_SEL   PIN_B4  // Select between 2 MOSI lines

// SPI port 2: ADS7952
#pin_select SDI2     = PIN_B15 // MISO
#pin_select SDO2     = PIN_B14 // MOSI
#pin_select SCK2OUT  = PIN_B13 // SCK
#define     ADC1_SEL   PIN_B12 // ADC-1, cells 1-12
#define     ADC2_SEL   PIN_B13 // ADC-2, cells 13-24

// CANbus port (unavailable on PIC24HJ128GP502)
//#pin_select C1TX     = PIN_B6  // CANTX pin
//#pin_select C1RX     = PIN_B7  // CANRX pin

// Miscellaneous pins
#define TEST_LED1      PIN_A0  // Test LED (green)
#define TEST_LED2      PIN_B5  // Test LED (red)
#define WARNING_LED    PIN_B7  // Warning LED (green)
#define CRITICAL_LED   PIN_B6  // Critical LED (red)
#define FAN_PIN        PIN_A4  // Fan PWM signal (LED on the prototype)
