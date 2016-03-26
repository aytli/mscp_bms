#ifndef LCD_C
#define LCD_C

// NOTE: In order to pass strings, add the following line to main.h
// #device PASS_STRINGS = IN_RAM
// Source: https://www.ccsinfo.com/forum/viewtopic.php?t=44633

// datasheet: http://www.kyocera-display.com/SiteImages/PartList/SPEC/51847ad%C3%A9%C3%A0.pdf
// lcd interface tutorial: http://www.8051projects.net/lcd-interfacing/lcd-4-bit.php

#define COMMAND_REG    0
#define DATA_REG       1
#define N_BITS         4 // using a 4 bit interface
#define LCD_BUSY_DELAY 5 // time spent waiting while the lcd is busy

static int g_data_pin[4] = {D4_PIN, D5_PIN, D6_PIN, D7_PIN};
static int g_row_address[4] = {0x00, 0x40, 0x14, 0x54};

// Writes a nibble to the LCD parallel interface
void lcd_send_nibble(int8 data)
{
    int i;
    
    output_low(RW_PIN); // we are writing so RW pin should be low
    output_low(EN_PIN); // make sure that the EN pin is low

    // load the nibble to the parallel interface on the lcd
    for (i = 0 ; i < N_BITS ; i++)
    {
        output_bit(g_data_pin[i], !!(data & (1 << i)));
    }
    
    // write operation is triggered by a falling edge of the EN pin
    output_high(EN_PIN);
    delay_us(1);
    output_low(EN_PIN);
}

// Writes to the data register on the LCD
void lcd_send_data(int8 data)
{
    output_low(RS_PIN);
    delay_ms(LCD_BUSY_DELAY); //wait until not busy
    
    output_bit(RS_PIN, DATA_REG); // we are sending data
    
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
}

// Writes to the command register on the LCD
void lcd_send_command(int8 data)
{
    output_low(RS_PIN);
    delay_ms(LCD_BUSY_DELAY); //wait until not busy
    
    output_bit(RS_PIN, COMMAND_REG); // we are sending a command
    
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
}

// Initializes the LCD for 4 bit communication
void lcd_init(void)
{
    // lcd reset sequence (http://www.8051projects.net/lcd-interfacing/lcd-4-bit.php)
    output_low(RS_PIN);
    delay_ms(20);
    lcd_send_command(0x30);
    delay_ms(10);
    lcd_send_command(0x30);
    delay_ms(1);
    lcd_send_command(0x30);
    delay_ms(1);
    lcd_send_command(0x20);
    delay_ms(1);
    
    // lcd initialization
    lcd_send_command(0x28); // 4 bit interface, 2 lines, 5x7 resolution
    lcd_send_command(0x0C); // display on, no cursor
    lcd_send_command(0x01); // clear display
    lcd_send_command(0x06); // automatically increment cursor
}

// Sets the cursor position on the LCD
void lcd_set_cursor_position(int8 row, int8 column)
{
    lcd_send_command((1 << 7) | (g_row_address[row] + column));
}

// Clears the LCD display
void lcd_clear(void)
{
    lcd_send_command(0x01);
}

// Writes a string to the LCD at the current cursor location
void lcd_write(char * str)
{
    int i = 0;
    while (*(str+i) != 0)
    {
        lcd_send_data(*(str+i));
        i++;
    }
}

#endif

