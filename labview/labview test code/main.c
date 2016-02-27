#include <main.h>

#define HEARTBEAT   PIN_A0
#define FAN         PIN_C4
#define VOLTAGE_ID  0x5A
#define TEMP_ID     0x69

void main()
{
    int i = 0;
    int n = 65;
    int m = 97;
    int pwm;
    setup_adc(ADC_CLOCK_INTERNAL);
    setup_adc_ports(ALL_ANALOG);
    set_adc_channel(1);
    
    while(true)
    {
        delay_ms(200);
        putc(VOLTAGE_ID);
        for (i = 0 ; i < 3 ; i++)
        {
            putc(0x01);
            putc(n+i);
        }
        for (i = 0 ; i < 3 ; i++)
        {
            putc(0x02);
            putc(2*n+i);
        }
        for (i = 0 ; i < 3 ; i++)
        {
            putc(0x03);
            putc(n+i);
        }
        (n < 90) ? (n++) : (n=65);
        
        delay_ms(200);
        putc(TEMP_ID);
        for (i = 0 ; i < 8 ; i++)
        {
            putc(m);
        }
        (m < 122) ? (m++) : (m=97);
        
        
        /*pwm = read_adc();
        pwm = (int)((float)(pwm*10.0)/256);
        printf("\r\n%u",pwm);
        delay_ms(pwm);
        output_high(HEARTBEAT);
        output_high(FAN);
        delay_ms(10-pwm);
        output_low(HEARTBEAT);
        output_low(FAN);*/
    }
}
