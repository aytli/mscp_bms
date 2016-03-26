#ifndef ADC_C
#define ADC_C

#define N_ADC_CHANNELS      24
#define LSBS_PER_VOLT       1820.44 // V_REF (Nominally 2.5V) / 4096 bits
#define THERMISTOR_NOMINAL  2500.0
#define TEMPERATURE_NOMINAL 25.0
#define THERMISTOR_SUPPLY   3.3
#define THERMISTOR_SERIES   10000.0
#define B_COEFF             4300.0

// Configures the ADS7952 to operate in Auto-1 Mode
void ads7952_init(void)
{
    // For the first frame the ADS7952 is in manual mode channel 0
    output_low(ADC1_SEL);
    spi_write2(0x00);
    spi_write2(0x00);
    output_high(ADC1_SEL);

    // Configure the Auto-1 Mode register
    // This mode will cycle through all of the channels automatically.
    output_low(ADC1_SEL);
    spi_write2(0x8F);
    spi_write2(0xFF);
    output_high(ADC1_SEL);
    output_low(ADC1_SEL);
    spi_write2(0xFF);
    spi_write2(0xFF);
    output_high(CSBI2);
}

// Reads all the channel voltages
// Expects an array of size 12 as an input
void ads7952_read_all_channels(unsigned int16* buf)
{
    int i;
    int ch;
    int msb, lsb;
    
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
        buf[ch] = ((msb & 0x0F) << 8 ) | lsb;
    }
}

#endif
