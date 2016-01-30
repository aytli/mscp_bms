#ifndef ADC_C
#define ADC_C

#define N_ADC_CHANNELS      24
#define LSBS_PER_VOLT       819.2
#define THERMISTOR_NOMINAL  10000.0
#define TEMPERATURE_NOMINAL 25.0
#define THERMISTOR_SUPPLY   3.3
#define THERMISTOR_SERIES   10000.0
#define B_COEFF             3950.0

// Configures the ADS7952 to operate in Auto-1 Mode
void ads7952_init(void)
{
   // For the first frame the ADS7952 is in manual mode channel 0
   output_low(CSBI2);
   spi_write2(0x00);
   spi_write2(0x00);
   output_high(CSBI2);

   // Configure the Auto-1 Mode register
   // This mode will cycle through all of the channels automatically.
   output_low(CSBI2);
   spi_write2(0x8F);
   spi_write2(0xFF);
   output_high(CSBI2);
   output_low(CSBI2);
   spi_write2(0xFF);
   spi_write2(0xFF);
   output_high(CSBI2);

   unsigned int8 data;
   while (1) {
      output_low(CSBI2);
      data = spi_read2(0x20);
      spi_read2(0x00);
      output_high(CSBI2);

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
   unsigned int8 data1, data2;

   for (ch = 0; ch < 12; ch++) {
      output_low(CSBI2);
      if (ch == 0) {
         data1 = spi_read2(0x24);
      } else {
         data1 = spi_read2(0x20);
      }

      // The power down bit must be set 1 frame before the last frame.
      // The chip powers down after the 16-th falling edge of SCK.
      if (ch == 10) {
         data2 = spi_read2(0x10);
      } else {
         data2 = spi_read2(0x00);
      }
      output_high(CSBI2);

      buf[ch] = ((data1 << 8) & 0xFF00) | (data2 & 0x00FF);
   }
}

#endif
