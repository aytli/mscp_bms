// 2Kb I2C serial CMOS eeprom: CAT24AA02
// Datasheet: http://www.onsemi.com/pub_link/Collateral/CAT24AA01-D.PDF

#define I2C_WRITE_BIT 0
#define I2C_READ_BIT  1

// I2C address of the device, 0b101000 + LSB
#define DEVICE_ADDRESS  0x60

// The device has memory addresses from 0x00 to 0x100
#define ERROR_ADDRESS   0x00
#define CELL_ID_ADDRESS 0x01

// 8-bit error codes
typedef enum
{
    OV_ERROR = 0x01,
    UV_ERROR = 0x02,
    OT_ERROR = 0x03,
    OC_ERROR = 0x04,
    UC_ERROR = 0x05,
    SUCCESS  = 0xFF
} eeprom_error_code_t;

// Writes an error code to the eeprom
void eeprom_write_error(eeprom_error_code_t error, int cell_id)
{
    output_low(WP_PIN);
    
    // Write the error code
    i2c_start();
    i2c_write(DEVICE_ADDRESS|I2C_WRITE_BIT);
    i2c_write(ERROR_ADDRESS);
    i2c_write((int8)(error));
    i2c_stop();
    
    // Write the cell id
    i2c_start();
    i2c_write(DEVICE_ADDRESS|I2C_WRITE_BIT);
    i2c_write(CELL_ID_ADDRESS);
    i2c_write((int8)(error));
    i2c_stop();
    
    output_high(WP_PIN);
}

// Reads an error code from the eeprom
eeprom_error_code_t eeprom_read_error(void)
{
    eeprom_error_code_t error;
    
    // Write the data address to the eeprom
    output_low(WP_PIN);
    i2c_start();
    i2c_write(DEVICE_ADDRESS|I2C_WRITE_BIT);
    i2c_write(CELL_ID_ADDRESS);
    output_high(WP_PIN);
    
    // Read a byte from the data address
    i2c_start();
    i2c_write(DEVICE_ADDRESS|I2C_READ_BIT);
    error = (eeprom_error_code_t)(i2c_read());
    i2c_stop();
    
    return error;
}

// Reads an error code from the eeprom
int eeprom_read_id(void)
{
    int id;
    
    // Write the data address to the eeprom
    output_low(WP_PIN);
    i2c_start();
    i2c_write(DEVICE_ADDRESS|I2C_WRITE_BIT);
    i2c_write(CELL_ID_ADDRESS);
    output_high(WP_PIN);
    
    // Read a byte from the data address
    i2c_start();
    i2c_write(DEVICE_ADDRESS|I2C_READ_BIT);
    id = i2c_read();
    i2c_stop();
    
    return id;
}

void eeprom_clear(void)
{
    eeprom_write_error(SUCCESS, 0xFF);
}
