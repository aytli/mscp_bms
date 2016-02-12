#include <main.h>

#define HEARTBEAT PIN_A0

void main()
{
    int i = 0;
    int n = 33;
    setup_adc_ports(sAN0);
    setup_timer_3(T3_DISABLED|T3_DIV_BY_1);
    setup_timer_4(T4_DISABLED,0,1);
    setup_comparator(NC_NC_NC_NC);// This device COMP currently not supported by the PICWizard

    while(true)
    {
        for (i = 0 ; i < 10 ; i++)
        {
            putc(n);
            delay_ms(10);
        }
        
        if (n < 126)
        {
            n++;
        }
        else
        {
            n = 33;
        }
        
        putc(0xFF);
        output_high(HEARTBEAT);
        output_low(HEARTBEAT);
        delay_ms(10);
    }
}
