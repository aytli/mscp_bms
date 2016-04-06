#ifndef FAN_C
#define FAN_C

#define FAN_PERIOD 10

enum fan_speed_t
{
    FAN_OFF  = 0,
    FAN_LOW  = (int)((float)(FAN_PERIOD*0.3)),
    FAN_HIGH = (int)((float)(FAN_PERIOD*0.7)),
    FAN_MAX  = FAN_PERIOD,
};

static fan_speed_t g_speed = 0;

// Set up timer 2 as a millisecond timer
int8 ms;
#int_timer3
void isr_timer3(void)
{
    ms++; //keep a running timer interupt that increments every milli-second

    if (ms <= g_speed)
    {
        output_high(FAN_PIN);
    }
    else if (ms <= FAN_PERIOD)
    {
        output_low(FAN_PIN);
    }
    else
    {
        ms = 0;
    }
}

void fan_init(void)
{
    // Set up and enable timer 3 to interrupt every 1ms using 20MHz clock
    setup_timer3(TMR_INTERNAL|TMR_DIV_BY_256,39);
    enable_interrupts(INT_TIMER3);
    g_speed = FAN_OFF;
}

void fan_set_speed(fan_speed_t speed)
{
    g_speed = speed;
}

#endif
