#include "pec.c"

// Sends an 11 bit (2 bytes) command to LTC-1
void ltc6804_send_command(unsigned int16 command)
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

// Sends an 11 bit (2 bytes) command to LTC-2
void ltc6804_send_command2(int16 command)
{
    spi_write2((command&(0xFF00))>>8);
    spi_write2(command&(0x00FF));
}

// Sends two bytes of config data to LTC-1
void ltc6804_send_config(int16 data)
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

    ltc6804_send_command(WRCFG);
    spi_write(CFGR0);
    spi_write(CFGR1);
    spi_write(CFGR2);
    spi_write(CFGR3);
    spi_write(data&0x00FF);
    spi_write((data&0xFF00)>>8);
    spi_write((crc&0xFF00)>>8);
    spi_write(crc&0x00FF);
}

// Sends two bytes of config data to LTC-2
void ltc6804_send_config2(int16 data)
{
    ltc6804_send_command2(WRCFG);
    spi_write2(CFGR0);
    spi_write2(CFGR1);
    spi_write2(CFGR2);
    spi_write2(CFGR3);
    spi_write2((data&0xFF00)>>8);
    spi_write2(data&0x00FF);
}

// Sends configuration bytes to LTC-1 and LTC-2
void ltc6804_init(void)
{
    output_low(CSBI1);
    output_low(CSBI2);
    ltc6804_send_config(0x0000);
    ltc6804_send_config2(0x0000);
    output_high(CSBI1);
    output_high(CSBI2);
}

// Starts the cell voltage ADC conversion
void ltc6804_start_cell_voltage_adc_conversion(void)
{
    output_low(CSBI1);
    ltc6804_send_command(ADCV);
    ltc6804_send_command(0xAF42);
    output_high(CSBI1);
}

// Receives a pointer to an array of cells, writes the cell voltage to each one
void ltc6804_read_cell_voltages(cell_t * cell)
{
    int i,msb = 0x69,lsb = 0xFF;
    
    // Start the cell voltage adc conversion
    ltc6804_start_cell_voltage_adc_conversion();
    
    // Wait 1ms for the conversion to complete
    delay_ms(1);
    
    output_low(CSBI1);
    
    ltc6804_send_command(RDCVA); // voltage data for cells 1-3
    for (i = 0 ; i < 3 ; i ++)
    {
        //lsb = spi_read();
        //msb = spi_read();
        cell[i].voltage = (msb << 8) + lsb;
    }
    
    ltc6804_send_command(RDCVB); // voltage data for cells 4-6
    for (i = 3 ; i < 6 ; i ++)
    {
        //lsb = spi_read();
        //msb = spi_read();
        cell[i].voltage = (msb << 8) + lsb;
    }
    
    // Prototype only contains 4 cells, no need to measure cells 7-12
    /*ltc6804_send_command(RDCVC); // voltage data for cells 7-9
    for (i = 6 ; i < 9 ; i ++)
    {
        lsb = spi_read();
        msb = spi_read();
        cell[i]->voltage = (msb << 8) + lsb;
    }
    
    ltc6804_send_command(RDCVD); // voltage data for cells 10-12
    for (i = 9 ; i < 12 ; i ++)
    {
        lsb = spi_read();
        msb = spi_read();
        cell[i]->voltage = (msb << 8) + lsb;
    }*/
    
    output_high(CSBI1);
}

// Receives a pointer to an array of cells, sets ov and uv flags for each one
void ltc6804_read_voltage_flags(cell_t * cell)
{
    int i,data;

    output_low(CSBI1);
    
    ltc6804_send_command(RDSTATB); // voltage flags are stored in register group B
    data = spi_read(); // 1st byte = digital power supply voltage lsb, useless
    data = spi_read(); // 2nd byte = digital power supply voltage msb, useless
    
    data = spi_read(); // 3rd byte = ov and uv flags for cells 1-4
    for (i = 0 ; i < 3 ; i+=2)
    {
        cell[i].uv_flag = (data & (1 <<  i   )) >>  i;
        cell[i].ov_flag = (data & (1 << (i+1))) >> (i+1);
    }
    
    data = spi_read(); // 4th byte = ov and uv flags for cells 5-8
    for (i = 4 ; i < 7 ; i+=2)
    {
        cell[i].uv_flag = (data & (1 <<  i   )) >>  i;
        cell[i].ov_flag = (data & (1 << (i+1))) >> (i+1);
    }
    
    data = spi_read(); // 5th byte = ov and uv flags for cells 9-12
    for (i = 8 ; i < 11 ; i+=2)
    {
        cell[i].uv_flag = (data & (1 <<  i   )) >>  i;
        cell[i].ov_flag = (data & (1 << (i+1))) >> (i+1);
    }
    
    data = spi_read(); // 6th byte = revision code and other useless bits
    
    output_high(CSBI1);
}
