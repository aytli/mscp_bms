#ifndef HALLSENSOR_C
#define HALLSENSOR_C

// Practice pack calibration
//#define CURRENT_CHARGE_MAX    2546  //~+40 A
//#define CURRENT_DISCHARGE_MAX 1031  //~-80 A
#define CURRENT_ZERO           2055
#define CURRENT_SLOPE         12.64

#define HALL_ADC_CHANNEL         24
#define HALL_TEMPERATURE_CHANNEL 25
#define HALL_ANALOG_PIN       sAN24
#define HALL_TEMPERATURE_PIN  sAN25

#define N_CURRENT_SAMPLES 10

typedef struct
{
    unsigned int16 raw;
    unsigned int16 samples[N_CURRENT_SAMPLES];
    unsigned int16 average;
} current_t;

// Initializes the hall effect sensor interface
void hall_sensor_init(void)
{
    setup_adc(ADC_CLOCK_INTERNAL);
    setup_adc_ports(HALL_ANALOG_PIN|HALL_TEMPERATURE_PIN);
}

// Returns the calibrated current value from the raw adc reading
float hall_sensor_adjust_current(unsigned int16 raw_current)
{
   return ((float)raw_current - CURRENT_ZERO)/CURRENT_SLOPE;
}

// Returns 1 if current_data is a positive current reading, 0 if negative
unsigned int8 hall_sensor_discharge(unsigned int16 current_data)
{
    return (current_data < CURRENT_ZERO);
}

// Retrieves and returns uint16 raw current value from hall effect sensor
unsigned int16 hall_sensor_read_data(void)
{
    set_adc_channel(HALL_ADC_CHANNEL);
    delay_us(10);
    return((unsigned int16)(read_adc()));
}

unsigned int16 hall_sensor_read_temperature(void)
{
    set_adc_channel(HALL_TEMPERATURE_CHANNEL);
    delay_us(10);
    return((unsigned int16)(read_adc()));
}

#endif
