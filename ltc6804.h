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
#define CFGR0   0x08   // VREFON = 1, ADCOPT = 0
#define CFGR1   0xD6   // Undervoltage = 2.80V (0x6D6)
#define CFGR2   0x06   // Overvoltage lower nibble + undervoltage upper nibble
#define CFGR3   0xA4   // Overvoltage  = 4.20V (0xA40)

// Number of channels on the LTC6804, and number of channels being used
#define N_CHANNELS 12 // The LTC6804 can monitor up to 12 cells
#define N_CELLS    4  // Number of cells actually connected

// Struct for a cell
typedef struct
{
    unsigned int16 voltage; // LTC6804 has a 16 bit voltage ADC
    int8 temperature;
    int8 ov_flag;
    int8 uv_flag;
    int8 ot_flag;
} cell_t;

void ltc6804_send_command(unsigned int16);
void ltc6804_send_command2(int16);
void ltc6804_send_config(int16);
void ltc6804_send_config2(int16);
void ltc6804_init(void);
void ltc6804_start_cell_voltage_adc_conversion(void);
void ltc6804_read_cell_voltages(cell_t *);
void ltc6804_read_voltage_flags(cell_t *);
