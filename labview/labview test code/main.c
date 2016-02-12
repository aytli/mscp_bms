#include <main.h>

#define HEARTBEAT   PIN_A0
#define VOLTAGE_ID  0x5A
#define TEMP_ID     0x69

void main()
{
    int i = 0;
    int n = 65;
    int m = 97;
    setup_adc_ports(sAN0);
    setup_timer_3(T3_DISABLED|T3_DIV_BY_1);
    setup_timer_4(T4_DISABLED,0,1);
    setup_comparator(NC_NC_NC_NC);// This device COMP currently not supported by the PICWizard

    while(true)
    {
        delay_ms(200);
        putc(VOLTAGE_ID);
        for (i = 0 ; i < 10 ; i++)
        {
            putc(n);
        }
        (n < 90) ? (n++) : (n=65);
        
        delay_ms(200);
        putc(TEMP_ID);
        for (i = 0 ; i < 10 ; i++)
        {
            putc(m);
        }
        (m < 122) ? (m++) : (m=97);
    }
}
