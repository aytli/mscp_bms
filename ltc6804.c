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
#define N_CELLS 24       // The LTC6804 can monitor up to 12 cells
#define N_CELLS_FINAL 30 // The number of cells in the final pack

// Number of samples for moving average
#define N_SAMPLES  10

// Voltage threshold for balancing to occur (BALANCE_THRESHOLD / 100) V
#define BALANCE_THRESHOLD 140

static int16 g_discharge1;
static int16 g_discharge2;

// Struct for a cell
typedef struct
{
    unsigned int16 voltage; // LTC6804 has a 16 bit voltage ADC
    unsigned int16 average_voltage;
    unsigned int16 samples[N_SAMPLES];
    int8 temperature;
    int8 ov_flag;
    int8 uv_flag;
    int8 ot_flag;
    int16 discharge;
} cell_t;

// Function prototypes
void ltc6804_wakeup(void);
void ltc6804_write_command(unsigned int16);
void ltc6804_write_config(int16,int16);
void ltc6804_init(void);
void ltc6804_read_cell_voltages(cell_t *);
void ltc6804_read_voltage_flags(cell_t *);
void ltc6804_start_discharge(int8 cell);
void ltc6804_stop_discharge(int8 cell);
void ltc6804_stop_all_discharge(void);

void ltc6804_wakeup(void)
{
    //Wake up serial interface
    output_low(CSBI1);
    output_low(CSBI2);
    delay_us(2);
    output_high(CSBI1);
    output_high(CSBI2);
    delay_us(14);
    output_low(CSBI1);
    output_low(CSBI2);
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
    
    output_low(CSBI1);
    ltc6804_write_config(g_discharge1);
    output_high(CSBI1);
    output_low(CSBI2);
    ltc6804_write_config(g_discharge2);
    output_high(CSBI2);
}

// Receives a pointer to an array of cells, writes the cell voltage to each one
void ltc6804_read_cell_voltages(cell_t * cell)
{
    int i, msb, lsb;
    
    // Start the cell voltage adc conversion
    output_low(CSBI1);
    ltc6804_write_command(ADCV);
    output_high(CSBI1);
    output_low(CSBI2);
    ltc6804_write_command(ADCV);
    output_high(CSBI2);
    
    // Wait 500us for the conversion to complete
    delay_us(500);
    
    // Read data for cells 0-2 from LTC-1
    output_low(MOSI_SEL);
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
    output_high(MOSI_SEL);
    
    // Read data for cells 12-14 from LTC-2
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
    
    delay_us(10);
    
    // Read data for cells 3-5 from LTC-1
    output_low(MOSI_SEL);
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
    output_high(MOSI_SEL);
    
    // Read data for cells 15-17 from LTC-1
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
}

// Receives a pointer to an array of cells, sets ov and uv flags for each one
void ltc6804_read_voltage_flags(cell_t * cell)
{
    int i,data;

    output_low(CSBI1);
    
    ltc6804_write_command(RDSTATB); // voltage flags are stored in register group B
    spi_read(0xFF); // 1st byte = digital power supply voltage lsb, useless
    spi_read(0xFF); // 2nd byte = digital power supply voltage msb, useless
    
    data = spi_read(0xFF); // 3rd byte = ov and uv flags for cells 1-4
    for (i = 0 ; i < 3 ; i+=2)
    {
        cell[i].uv_flag = (data & (1 <<  i   )) >>  i;
        cell[i].ov_flag = (data & (1 << (i+1))) >> (i+1);
    }
    
    data = spi_read(0xFF); // 4th byte = ov and uv flags for cells 5-8
    for (i = 4 ; i < 7 ; i+=2)
    {
        cell[i].uv_flag = (data & (1 <<  i   )) >>  i;
        cell[i].ov_flag = (data & (1 << (i+1))) >> (i+1);
    }
    
    data = spi_read(0xFF); // 5th byte = ov and uv flags for cells 9-12
    for (i = 8 ; i < 11 ; i+=2)
    {
        cell[i].uv_flag = (data & (1 <<  i   )) >>  i;
        cell[i].ov_flag = (data & (1 << (i+1))) >> (i+1);
    }
    
    spi_read(0xFF); // 6th byte = revision code and other useless bits
    spi_read(0xFF); // PEC 1
    spi_read(0xFF); // PEC 2
    
    output_high(CSBI1);
}

// Starts discharge for a cell
// Receives the index of the cell (from 0 to 23)
void ltc6804_start_discharge(int8 cell)
{
    if (cell <= 11)
    {
        // Cell is in LTC-1
        g_discharge1 |= 1 << cell;
        output_low(CSBI1);
        ltc6804_write_config(g_discharge1);
        output_high(CSBI1);
    }
    else if (cell <= 23)
    {
        // Cell is in LTC-2
        g_discharge2 |= 1 << (cell-12);
        output_low(CSBI1);
        ltc6804_write_config(g_discharge2);
        output_high(CSBI1);
    }
    else
    {
        // Cell is out of range, do not send anything
        return;
    }
}

// Stops discharge for a cell
// Receives the index of the cell (from 0 to 23)
void ltc6804_stop_discharge(int8 cell)
{
    if (cell <= 11)
    {
        // Cell is in LTC-1
        g_discharge1 &= ~(1 << cell);
        output_low(CSBI1);
        ltc6804_write_config(g_discharge1);
        output_high(CSBI1);
    }
    else if (cell <= 23)
    {
        // Cell is in LTC-2
        g_discharge2 &= ~(1 << (cell-12));
        output_low(CSBI2);
        ltc6804_write_config(g_discharge2);
        output_high(CSBI2);
    }
    else
    {
        // Cell is out of range, do not send anything
        return;
    }
}

// Stops discharge for all cells
void ltc6804_stop_all_discharge(void)
{
    ltc6804_init();
}

#endif
