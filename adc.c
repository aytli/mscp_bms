#ifndef ADC_C
#define ADC_C

#define N_ADC_CHANNELS      24
#define LSBS_PER_VOLT       1820.44 // V_REF (Nominally 2.5V) / 4096 bits
#define THERMISTOR_NOMINAL  10000.0
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

   unsigned int8 data;
   while (1) {
      output_low(ADC1_SEL);
      data = spi_read2(0x20);
      spi_read2(0x00);
      output_high(ADC1_SEL);

      /*
       * This makes sure that the next frame will contain conversion data
       * for channel 0. If this is not included, there will be synchronization
       * issues.
       */
      if (((data >> 4) & 0x0F) == 0x0B) {
         break;
      }
   }
}

// Reads all the channel voltages
// Expects an array of size 12 as an input
void ads7952_read_all_channels(unsigned int16* buf)
{
   int ch;
   int msb, lsb;

   for (ch = 0; ch < 12; ch++) {
      output_low(ADC1_SEL);
      if (ch == 0) {
         msb = spi_read2(0x24);
      } else {
         msb = spi_read2(0x20);
      }

      // The power down bit must be set 1 frame before the last frame.
      // The chip powers down after the 16-th falling edge of SCK.
      if (ch == 10) {
         lsb = spi_read2(0x10);
      } else {
         lsb = spi_read2(0x00);
      }
      output_high(ADC1_SEL);

      buf[ch] = ((msb & 0xF) << 8 ) | lsb;
   }
}

#endif
