// Practice pack calibration
#define CURRENT_CHARGE_MAX    2546  //~+40 A
#define CURRENT_DISCHARGE_MAX 1031  //~-80 A
#define CURRENT_ZERO          2043
#define CURRENT_SLOPE         12.64

#define HALL_ADC_CHANNEL 3

typedef struct
{
    unsigned int16 data;
    unsigned int8  overCount;
    unsigned int8  overFlag;
} hall_data_t;

float rawToCurr(unsigned int16 rawCurr)
{
   return ((float)rawCurr - CURRENT_ZERO)/CURRENT_SLOPE;
}

/*
 *  hallDischarge
 *
 *  Return:
 *  1 if the hall sensor measures positive current, 0 otherwise
 *
 */
unsigned int8 hallDischarge(void)
{
    return (hallSensor.data < CURRENT_ZERO);
}

//Retrieves and returns uint16 raw current value from hall effect sensor
unsigned int16 getRawCurrentVal(void)
{
    unsigned int16 raw_current = read_adc();
    return(raw_current);
}
