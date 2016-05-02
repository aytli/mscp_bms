#ifndef EEPROM_C
#define EEPROM_C

// 2Kb I2C serial CMOS eeprom: CAT24AA02
// Datasheet: http://www.onsemi.com/pub_link/Collateral/CAT24AA01-D.PDF

#define I2C_WRITE_BIT 0
#define I2C_READ_BIT  1

// I2C address of the device, 0b101000 + LSB
#define DEVICE_ADDRESS  0xA0

// The device has memory addresses from 0x00 to 0x100
#define BASE_ADDRESS    0x00
#define OV_ADDRESS      0x00
#define UV_ADDRESS      0x04
#define OT_ADDRESS      0x08
#define CURRENT_ADDRESS 0x0B

// The eeprom will store 4 bytes of error data
#define N_ERROR_BYTES 4

// The EEPROM takes 5ms to write data to memory
#define WRITE_TIME_MS 5

#define EEPROM_SUCCESS 0xFF

typedef enum
{
    OC_ERROR        = 1,
    UC_ERROR        = 2,
} current_error_t;

static int8 g_ov_error = EEPROM_SUCCESS;
static int8 g_uv_error = EEPROM_SUCCESS;
static int8 g_ot_error = EEPROM_SUCCESS;
static int8 g_current_error = EEPROM_SUCCESS;

// Writes an error code to the eeprom
void eeprom_write_errors(void)
{
    // Write the error code
    output_low(WP_PIN);
    i2c_start();
    i2c_write(DEVICE_ADDRESS|I2C_WRITE_BIT);
    i2c_write(BASE_ADDRESS);
    i2c_write(g_ov_error);              // Byte 0 - OV error
    i2c_write(g_uv_error);              // Byte 1 - UV error
    i2c_write(g_ot_error);              // Byte 2 - OT error
    i2c_write((int8)(g_current_error)); // Byte 3 - Current error
    i2c_stop();
    output_high(WP_PIN);
    delay_ms(WRITE_TIME_MS);
}

// Reads the contents of the eeprom
void eeprom_read(int8 * data)
{
    output_low(WP_PIN);
    i2c_start();
    i2c_write(DEVICE_ADDRESS|I2C_WRITE_BIT);
    i2c_write(BASE_ADDRESS);
    i2c_start();
    i2c_write(DEVICE_ADDRESS|I2C_READ_BIT);
    *(data+0) = i2c_read(1); // OV error, ACK
    *(data+1) = i2c_read(1); // UV error, ACK
    *(data+2) = i2c_read(1); // OT error, ACK
    *(data+3) = i2c_read(0); // current error, NOACK, stop
    i2c_stop();
    output_high(WP_PIN);
}

void eeprom_clear(void)
{
    g_ov_error = EEPROM_SUCCESS;
    g_uv_error = EEPROM_SUCCESS;
    g_ot_error = EEPROM_SUCCESS;
    g_current_error = EEPROM_SUCCESS;
    eeprom_write_errors();
}

void eeprom_set_ov_error(int8 id)
{
    g_ov_error = id;
}

void eeprom_set_uv_error(int8 id)
{
    g_uv_error = id;
}

void eeprom_set_ot_error(int8 id)
{
    g_ot_error = id;
}

void eeprom_set_current_error(current_error_t error)
{
    g_current_error = (int8)(error);
}

#endif
