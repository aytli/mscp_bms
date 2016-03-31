#ifndef HALLSENSOR_C
#define HALLSENSOR_C

// Practice pack calibration
#define CURRENT_CHARGE_MAX    2546  //~+40 A
#define CURRENT_DISCHARGE_MAX 1031  //~-80 A
#define CURRENT_ZERO          2043
#define CURRENT_SLOPE         12.64

#define HALL_ADC_CHANNEL 0

typedef struct
{
    unsigned int16 data;
    unsigned int8  overCount;
    unsigned int8  overFlag;
} hall_data_t;

float hall_sensor_adjust_current(unsigned int16 raw_current)
{
   return ((float)raw_current - CURRENT_ZERO)/CURRENT_SLOPE;
}

/*
 *  hallDischarge
 *
 *  Return:
 *  1 if the hall sensor measures positive current, 0 otherwise
 *
 */
unsigned int8 hall_sensor_discharge(void)
{
    return 0;
    //return (hallSensor.data < CURRENT_ZERO);
}

//Retrieves and returns uint16 raw current value from hall effect sensor
unsigned int16 hall_sensor_read_data(void)
{
    set_adc_channel(HALL_ADC_CHANNEL);
    delay_us(10);
    return((unsigned int16) (read_adc()));
}

#endif
