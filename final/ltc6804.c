#ifndef LTC6804_C
#define LTC6804_C

#include "pec.c"

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
#define ADCV    0x0370 // Datasheet page 53

// LTC6804 configuration bytes (bytes 4 and 5 used for charging/discharging)
#define CFGR0   0x00   // VREFON = 1, ADCOPT = 0
#define CFGR1   0xD6   // Undervoltage = 2.80V (0x6D6)
#define CFGR2   0x06   // Overvoltage lower nibble + undervoltage upper nibble
#define CFGR3   0xA4   // Overvoltage  = 4.20V (0xA40)

// Number of channels on the LTC6804, and number of channels being used
#define N_CELLS 30     // The 3 LTC devices will monitor 30 cells

// Number of samples for moving average
#define N_VOLTAGE_SAMPLES 10

// Voltage threshold for balancing to occur (BALANCE_THRESHOLD / 100) V
#define BALANCE_THRESHOLD 140

// Unintuitive, but the mux is actually configured in this way according
// to the schmatic:
//
// LTC_1: S[1:0] = 10 
// LTC_2: S[1:0] = 01
// LTC_3: S[1:0] = 00
//
#define SELECT_LTC_1        \
    output_low(MISO_SEL0);  \
    output_high(MISO_SEL1);

#define SELECT_LTC_2        \
    output_high(MISO_SEL0); \
    output_low(MISO_SEL1);

#define SELECT_LTC_3        \
    output_low(MISO_SEL0);  \
    output_low(MISO_SEL1);

static int16 g_discharge1;
static int16 g_discharge2;
static int16 g_discharge3;

// Struct for a cell
typedef struct
{
    unsigned int16 voltage; // LTC6804 has a 16 bit voltage ADC
    unsigned int16 average_voltage;
    unsigned int16 samples[N_VOLTAGE_SAMPLES];
} cell_t;

// Function prototypes
void ltc6804_wakeup(void);
void ltc6804_write_command(unsigned int16);
void ltc6804_write_config(int16,int16);
void ltc6804_init(void);
void ltc6804_read_cell_voltages(cell_t *);

void ltc6804_wakeup(void)
{
    //Wake up serial interface
    output_low(CSBI1);
    output_low(CSBI2);
    output_low(CSBI3);
    delay_us(2);
    output_high(CSBI1);
    output_high(CSBI2);
    output_high(CSBI3);
    delay_us(14);
    output_low(CSBI1);
    output_low(CSBI2);
    output_low(CSBI3);
}

// Sends an 11 bit (2 bytes) command to LTC-1
void ltc6804_write_command(unsigned int16 command)
{
    char bytes[2];
    unsigned int16 crc;
    bytes[0] = (command&(0xFF00))>>8;
    bytes[1] = command&(0x00FF);
    crc = pec15(bytes,2);
    spi_write((command&(0xFF00))>>8);
    spi_write(command&(0x00FF));
    spi_write((crc&0xFF00)>>8);
    spi_write(crc&0x00FF);
}

// Sends two bytes of config data to LTC-1
void ltc6804_write_config(int16 data)
{
    char bytes[6];
    unsigned int16 crc;
    bytes[0] = CFGR0;
    bytes[1] = CFGR1;
    bytes[2] = CFGR2;
    bytes[3] = CFGR3;
    bytes[4] = data&0x00FF;
    bytes[5] = (data&0xFF00)>>8;
    crc = pec15(bytes,6);

    ltc6804_write_command(WRCFG);
    spi_write(CFGR0);
    spi_write(CFGR1);
    spi_write(CFGR2);
    spi_write(CFGR3);
    spi_write(data&0x00FF);
    spi_write((data&0xFF00)>>8);
    spi_write((crc&0xFF00)>>8);
    spi_write(crc&0x00FF);
}

// Sends configuration bytes to LTC-1 and LTC-2
void ltc6804_init(void)
{
    g_discharge1 = 0x0000;
    g_discharge2 = 0x0000;
    g_discharge3 = 0x0000;
    
    init_PEC15_Table();
    ltc6804_wakeup();
    output_low(CSBI1);
    ltc6804_write_config(g_discharge1);
    output_high(CSBI1);
    output_low(CSBI2);
    ltc6804_write_config(g_discharge2);
    output_high(CSBI2);
    output_low(CSBI3);
    ltc6804_write_config(g_discharge3);
    output_high(CSBI3);
}

// Receives a pointer to an array of cells, writes the cell voltage to each one
void ltc6804_read_cell_voltages(cell_t * cell)
{
    int i;
    int msb;
    int lsb;
    
    // Start the cell voltage adc conversion
    output_low(CSBI1);
    ltc6804_write_command(ADCV);
    output_high(CSBI1);
    output_low(CSBI2);
    ltc6804_write_command(ADCV);
    output_high(CSBI2);
    output_low(CSBI3);
    ltc6804_write_command(ADCV);
    output_high(CSBI3);
    
    // Wait 500us for the conversion to complete
    delay_us(500);
    
    // Read data for cells 0-2 from LTC-1
    SELECT_LTC_1;
    output_low(CSBI1);
    ltc6804_write_command(RDCVA);
    for (i = 0 ; i < 3 ; i ++)
    {
        lsb = spi_read(0xFF);
        msb = spi_read(0xFF);
        cell[i].voltage = (msb<<8)+lsb;
    }    
    spi_read(0xFF); // PEC1
    spi_read(0xFF); // PEC2
    output_high(CSBI1);
    
    // Read data for cells 3-5 from LTC-1
    output_low(CSBI1);
    ltc6804_write_command(RDCVB);
    for (i = 3 ; i < 6 ; i ++)
    {
        lsb = spi_read(0xFF);
        msb = spi_read(0xFF);
        cell[i].voltage = (msb<<8)+lsb;
    }    
    spi_read(0xFF); // PEC1
    spi_read(0xFF); // PEC2
    output_high(CSBI1);
    
    // Read data for cells 6-8 from LTC-1
    output_low(CSBI1);
    ltc6804_write_command(RDCVC);
    for (i = 6 ; i < 9 ; i ++)
    {
        lsb = spi_read(0xFF);
        msb = spi_read(0xFF);
        cell[i].voltage = (msb<<8)+lsb;
    }    
    spi_read(0xFF); // PEC1
    spi_read(0xFF); // PEC2
    output_high(CSBI1);
    
    // Read data for cells 9-11 from LTC-1
    output_low(CSBI1);
    ltc6804_write_command(RDCVD);
    for (i = 9 ; i < 12 ; i ++)
    {
        lsb = spi_read(0xFF);
        msb = spi_read(0xFF);
        cell[i].voltage = (msb<<8)+lsb;
    }    
    spi_read(0xFF); // PEC1
    spi_read(0xFF); // PEC2
    output_high(CSBI1);
    
    // Read data for cells 12-14 from LTC-2
    SELECT_LTC_2;
    output_low(CSBI2);
    ltc6804_write_command(RDCVA);
    for (i = 12 ; i < 15 ; i ++)
    {
        lsb = spi_read(0xFF);
        msb = spi_read(0xFF);
        cell[i].voltage = (msb<<8)+lsb;
    }    
    spi_read(0xFF); // PEC1
    spi_read(0xFF); // PEC2
    output_high(CSBI2);
    
    // Read data for cells 15-17 from LTC-2
    output_low(CSBI2);
    ltc6804_write_command(RDCVB);
    for (i = 15 ; i < 18 ; i ++)
    {
        lsb = spi_read(0xFF);
        msb = spi_read(0xFF);
        cell[i].voltage = (msb<<8)+lsb;
    }    
    spi_read(0xFF); // PEC1
    spi_read(0xFF); // PEC2
    output_high(CSBI2);
    
    // Read data for cells 18-20 from LTC-2
    output_low(CSBI2);
    ltc6804_write_command(RDCVC);
    for (i = 18 ; i < 21 ; i ++)
    {
        lsb = spi_read(0xFF);
        msb = spi_read(0xFF);
        cell[i].voltage = (msb<<8)+lsb;
    }    
    spi_read(0xFF); // PEC1
    spi_read(0xFF); // PEC2
    output_high(CSBI2);
    
    // Read data for cells 21-23 from LTC-2
    output_low(CSBI2);
    ltc6804_write_command(RDCVD);
    for (i = 21 ; i < 24 ; i ++)
    {
        lsb = spi_read(0xFF);
        msb = spi_read(0xFF);
        cell[i].voltage = (msb<<8)+lsb;
    }    
    spi_read(0xFF); // PEC1
    spi_read(0xFF); // PEC2
    output_high(CSBI2);
    
    // Read data for cells 24-26 from LTC-3
    SELECT_LTC_3;
    output_low(CSBI3);
    ltc6804_write_command(RDCVA);
    for (i = 24 ; i < 27 ; i ++)
    {
        lsb = spi_read(0xFF);
        msb = spi_read(0xFF);
        cell[i].voltage = (msb<<8)+lsb;
    }    
    spi_read(0xFF); // PEC1
    spi_read(0xFF); // PEC2
    output_high(CSBI3);
    
    // Read data for cells 27-29 from LTC-3
    output_low(CSBI3);
    ltc6804_write_command(RDCVB);
    for (i = 27 ; i < 30 ; i ++)
    {
        lsb = spi_read(0xFF);
        msb = spi_read(0xFF);
        cell[i].voltage = (msb<<8)+lsb;
    }    
    spi_read(0xFF); // PEC1
    spi_read(0xFF); // PEC2
    output_high(CSBI3);
}

#endif
