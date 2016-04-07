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

typedef enum
{
    OC_ERROR        = 1,
    UC_ERROR        = 2,
    CURRENT_SUCCESS = 0xFF
} current_error_t;

static int8 g_ov_error;
static int8 g_uv_error;
static int8 g_ot_error;
static int8 g_current_error;

// Writes an error code to the eeprom
void eeprom_write_errors(void)
{
    // Write the error code
    i2c_start();
    i2c_write(DEVICE_ADDRESS|I2C_WRITE_BIT);
    i2c_write(BASE_ADDRESS);
    i2c_write(g_ov_error);              // Byte 0  - OV error
    i2c_write(g_uv_error);              // Byte 1  - UV error
    i2c_write(g_ot_error);              // Byte 2  - OT error
    i2c_write((int8)(g_current_error)); // Byte 3 - Current error
    i2c_stop();
}

// Reads the contents of the eeprom
void eeprom_read(int8 * data)
{
    int i;
    
    // Write the data address to the eeprom
    i2c_start();
    i2c_write(DEVICE_ADDRESS|I2C_WRITE_BIT);
    i2c_write(BASE_ADDRESS);
    
    // Read the contents of the eeprom memory
    i2c_start();
    i2c_write(DEVICE_ADDRESS|I2C_READ_BIT);
    
    for (i = 0 ; i < N_ERROR_BYTES ; i++)
    {
        *(data+i) = i2c_read(0);
    }
    
    i2c_stop();
}

void eeprom_clear(void)
{
    output_low(WP_PIN);
    g_ov_error = 0xFF;
    g_uv_error = 0xFF;
    g_ot_error = 0xFF;
    g_current_error = CURRENT_SUCCESS;
    eeprom_write_errors();
}

void eeprom_set_ov_error(int id)
{
    g_ov_error = id;
}

void eeprom_set_uv_error(int id)
{
    g_uv_error = id;
}

void eeprom_set_ot_error(int id)
{
    g_ot_error = id;
}

void eeprom_set_current_error(current_error_t error)
{
    g_current_error = (int8)(error);
}

#endif
