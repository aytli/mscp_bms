#ifndef ADC_C
#define ADC_C

// ADS7952 datasheet: http://www.ti.com/lit/ds/symlink/ads7952.pdf

// ADS7952 command codes
#define CONTINUE_OP     0b0000
#define MANUAL_MODE_CON 0b0001
#define AUTO1_MODE_CON  0b0010
#define AUTO2_MODE_CON  0b0011
#define GPIO_PROG       0b0100
#define AUTO1_MODE_PROG 0b1000
#define AUTO2_MODE_PROG 0b1001
#define ALARM0_PROG     0b1100
#define ALARM1_PROG     0b1101
#define ALARM2_PROG     0b1110
#define ALARM3_PROG     0b1111

void adc_init_manual(void)
{
    output_low(ADC1_SEL);
    spi_write2(0x18);
    spi_write2(0x00);
    output_high(ADC1_SEL);
}

/*unsigned int16 g_adc_channel[36];

void init_adcs()
{
   // Program all ADS795s for auto mode 1
   // ADCs will automatically cycle through all the channels
   output_low(ADC1_SEL);
   spi_write(0x8F);
   spi_write(0xFF);
   output_high(ADC1_SEL);
   output_low(ADC2_SEL);
   spi_write(0x8F);
   spi_write(0xFF);
   output_high(ADC2_SEL);
   output_low(ADC3_SEL);
   spi_write(0x8F);
   spi_write(0xFF);
   output_high(ADC3_SEL);
}

void update_channels()
{
   int i;
   output_low(ADC1_SEL);
   spi_read(0x24);
   spi_read(0x00);
   output_high(ADC1_SEL);
   for (i = 0; i < 12; i++) {
      output_low(ADC1_SEL);
      g_adc_channel[i] = spi_read(0x20) << 8;
      // Power down bit should be set on the second last read
      g_adc_channel[i] |= spi_read(i == 10 ? 0x10 : 0x00);
      output_high(ADC1_SEL);
   }
   
   output_low(ADC2_SEL);
   spi_read(0x24);
   spi_read(0x00);
   output_high(ADC2_SEL);
   for (i = 0; i < 12; i++) {
      output_low(ADC2_SEL);
      g_adc_channel[12 + i] = spi_read(0x20) << 8;
      g_adc_channel[12 + i] |= spi_read(i == 10 ? 0x10 : 0x00);
      output_high(ADC2_SEL);
   }
   
   output_low(ADC3_SEL);
   spi_read(0x24);
   spi_read(0x00);
   output_high(ADC3_SEL);
   for (i = 0; i < 12; i++) {
      output_low(ADC3_SEL);
      g_adc_channel[24 + i] = spi_read(0x20) << 8;
      g_adc_channel[24 + i] |= spi_read(i == 10 ? 0x10 : 0x00);
      output_high(ADC3_SEL);
   }
}*/

#endif

