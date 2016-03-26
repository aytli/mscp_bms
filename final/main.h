#include <24HJ256GP610A.h>

#device PASS_STRINGS = IN_RAM

#fuses WPRES32 // Watch Dog Timer PreScalar 1:32
#fuses WPOSTS1 // Watch Dog Timer PostScalar 1:1
#fuses CKSFSM  // Clock Switching is enabled, fail Safe clock monitor is enabled

// Use internal oscillator for delay
#use delay(internal = 20000000)

// UART port (PIC24HJ128GP502)
#use rs232(baud = 57600, UART1)

// SPI port 1: LTC6804-1
#use spi(SPI1)
#define CSBI1     PIN_D10 // LTC-1 chip select, active low
#define CSBI2     PIN_A3  // LTC-2 chip select, active low
#define CSBI3     PIN_A2  // LTC-3 chip select, active low
#define MOSI_SEL0 PIN_D14 // Select between 3 MOSI lines
#define MOSI_SEL1 PIN_D15

// SPI port 2: ADS7952
#use spi(SPI2)
#define ADC1_SEL  PIN_B8  // ADC-1, thermistors 0-11
#define ADC2_SEL  PIN_B9  // ADC-2, thermistors 12-23

// Miscellaneous pins
#define HEARTBEAT_LED1 PIN_E2  // Hearbeat and diagnostic LED
#define HEARTBEAT_LED2 PIN_G13 // Hearbeat and diagnostic LED
