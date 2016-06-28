#ifndef ADC_C
#define ADC_C

#define N_ADC_CHANNELS      24
#define LSBS_PER_VOLT       1820.44 // V_REF (Nominally 2.5V) / 4096 bits
#define THERMISTOR_NOMINAL  2500.0
#define TEMPERATURE_NOMINAL 25.0
#define THERMISTOR_SUPPLY   3.3
#define THERMISTOR_SERIES   10000.0
#define B_COEFF             4300.0

#define N_TEMPERATURE_SAMPLES 10

typedef struct
{
    unsigned int16 raw;
    unsigned int16 samples[N_TEMPERATURE_SAMPLES];
    unsigned int16 average;
    float          converted;
    unsigned int8  ot_count; // Critical temperature error counter
    unsigned int8  wt_count; // Temperature warning counter
} temperature_t;

// ADC channels on PCB are not mapped in order
// These two arrays are lookup tables to correctly map the channels

// ADC1:
// Channel number:   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11
// Cell number:     10,  9,  8,  7,  5,  4,  3,  2, 15, 14, 13, 12
// ADC index:        7,  6,  5,  4,  3,  2,  1,  0, 11, 10,  9,  8
static unsigned int8 g_channel_map1[12] = {7,6,5,4,3,2,1,0,11,10,9,8};

// ADC2:
// Channel number:  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
// Cell number:     20, 19, 18, 17, 22, 23, 24, 25, 27, 28, 29, 30
// ADC index:       15, 14, 13, 12, 16, 17, 18, 19, 20, 21, 22, 23
static unsigned int8 g_channel_map2[12] = {15,14,13,12,16,17,18,19,20,21,22,23};

// Configures the ADS7952 to operate in Auto-1 Mode
void ads7952_init(void)
{
    // For the first frame the ADS7952 is in manual mode channel 0
    output_low(ADC1_SEL);
    spi_write2(0x00);
    spi_write2(0x00);
    output_high(ADC1_SEL);
    output_low(ADC2_SEL);
    spi_write2(0x00);
    spi_write2(0x00);
    output_high(ADC2_SEL);
    
    // Configure the Auto-1 Mode register
    // This mode will cycle through all of the channels automatically.
    output_low(ADC1_SEL);
    spi_write2(0x8F);
    spi_write2(0xFF);
    output_high(ADC1_SEL);
    output_low(ADC1_SEL);
    spi_write2(0xFF);
    spi_write2(0xFF);
    output_high(ADC1_SEL);
    output_low(ADC2_SEL);
    spi_write2(0x8F);
    spi_write2(0xFF);
    output_high(ADC2_SEL);
    output_low(ADC2_SEL);
    spi_write2(0xFF);
    spi_write2(0xFF);
    output_high(ADC2_SEL);
}

// Reads all the channel voltages
// Expects an array of size 12 as an input
void ads7952_read_all_channels(temperature_t * adc)
{
    int i;
    int ch;
    int msb;
    int lsb;
    
    for (i = 0 ; i < 12 ; i ++)
    {
        output_low(ADC1_SEL);
        msb = spi_read2(0x20);
        
        // The power down bit must be set 1 frame before the last frame.
        // The chip powers down after the 16-th falling edge of SCK.
        if (i == 10)
        {
            lsb = spi_read2(0x10);
        }
        else
        {
            lsb = spi_read2(0x00);
        }
        output_high(ADC1_SEL);
        
        ch = msb >> 4;
        adc[g_channel_map1[ch]].raw = ((0x0F & msb) << 8 ) | lsb;
    }
    
    for (i = 12 ; i < 24 ; i ++)
    {
        output_low(ADC2_SEL);
        msb = spi_read2(0x20);
        
        // The power down bit must be set 1 frame before the last frame.
        // The chip powers down after the 16-th falling edge of SCK.
        if (i == 22)
        {
            lsb = spi_read2(0x10);
        }
        else
        {
            lsb = spi_read2(0x00);
        }
        output_high(ADC2_SEL);
        
        ch = (msb >> 4);
        adc[g_channel_map2[ch]].raw = ((0x0F & msb) << 8 ) | lsb;
    }
}

#endif
