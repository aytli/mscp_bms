#include <24HJ256GP610A.h>

#device PASS_STRINGS = IN_RAM
#device ADC = 12

#fuses WPRES32 // Watch Dog Timer PreScalar 1:32
#fuses WPOSTS1 // Watch Dog Timer PostScalar 1:1
#fuses CKSFSM  // Clock Switching is enabled, fail Safe clock monitor is enabled
#fuses HS      // High speed oscillator frequency

// Using external oscillator
#use delay(crystal = 20000000)

// UART port (PIC24HJ256GP610A)
#use rs232(baud = 57600, xmit = PIN_F2, rcv = PIN_F3)

// SPI port 1: LTC6804-1
#use spi(SPI1, BAUD = 125000, IDLE = 1, SAMPLE_RISE)
#define CSBI1     PIN_D10 // LTC-1 chip select, active low
#define CSBI2     PIN_A3  // LTC-2 chip select, active low
#define CSBI3     PIN_A2  // LTC-3 chip select, active low
#define MISO_SEL0 PIN_D14 // Selects between 3 MOSI lines
#define MISO_SEL1 PIN_D15 // Selects between 3 MOSI lines

// SPI port 2: ADS7952
#use spi(SPI2, BAUD = 125000)
#define ADC1_SEL  PIN_B8  // ADC-1, thermistors 0-11
#define ADC2_SEL  PIN_B9  // ADC-2, thermistors 12-23

// I2C port: CAT24AA02
#use i2c(MASTER, SCL = PIN_G2, SDA = PIN_G3)
#define WP_PIN    PIN_A6

// LCD interface (4 bit mode)
#define RS_PIN    PIN_C4
#define RW_PIN    PIN_E4
#define EN_PIN    PIN_C3
#define D4_PIN    PIN_C2
#define D5_PIN    PIN_C1
#define D6_PIN    PIN_E6
#define D7_PIN    PIN_E5
#define LCD_SIG   PIN_E7  // LCD_SIG will be high when the LCD is enabled

// Hall effect sensor
#define HALL_PIN  PIN_E0
#define HALL_TEMP PIN_E1

// Miscellaneous pins
#define STATUS    PIN_E3  // Status LED
#define TX_LED    PIN_E2  // Test LED (TX)
#define RX_LED    PIN_G13 // Test LED (RX)
#define KVAC_PIN  PIN_A13 // Kilovac control pin
#define FAN_PIN   PIN_D7  // Fan PWM signal

// State machine states
typedef enum
{
    SAFETY_CHECK,
    BEGIN_BALANCE,
    BALANCING,
    PMS_RESPONSE_PENDING,
    DISCONNECT_PACK,
    N_STATES
} bps_state;
