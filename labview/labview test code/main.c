#include <main.h>

#define HEARTBEAT_LED PIN_A0
#define VOLTAGE_ID    0x5A
#define TEMP_ID       0x69
#define BALANCE_ID    0x41
#define CURRENT_ID    0xE7
#define STATUS_ID     0x77

#define N_CELLS        30
#define N_ADC_CHANNELS 24

static int1 g_status = 0;

void send_voltage_data(void)
{
    int i;
    putc(VOLTAGE_ID);
    for (i = 0 ; i < N_CELLS ; i ++)
    {
        putc(0x8B);
        putc(0x55);
    }
}

void send_temperature_data(void)
{
    int i;
    putc(TEMP_ID);
    for (i = 0 ; i < N_ADC_CHANNELS ; i++)
    {
        putc(0x7E);
    }
}

void send_current_data(void)
{
    putc(CURRENT_ID);
    putc(0x0B);
    putc(0xFE);
}

void send_balancing_bits(void)
{
    int32 discharge = 0x00000000;
    putc(BALANCE_ID);
    putc((int8)(discharge&0xFF));
    putc((int8)((discharge>> 8)&0xFF));
    putc((int8)((discharge>>16)&0xFF));
    putc((int8)((discharge>>24)&0x3F));
}

void send_status(void)
{
    putc(STATUS_ID);
    putc(g_status);
}

void main()
{
    while(true)
    {
        send_voltage_data();
        delay_ms(10);
        send_temperature_data();
        delay_ms(10);
        send_current_data();
        delay_ms(10);
        send_balancing_bits();
        delay_ms(10);
        send_status();
        output_toggle(HEARTBEAT_LED);
        delay_ms(200);
        
        g_status = !g_status;
    }
}
