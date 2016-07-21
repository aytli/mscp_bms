#include <18F26K80.h>

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES SOSC_DIG                 //Digital mode, I/O port functionality of RC0 and RC1
#FUSES NOXINST                  //Extended set extension and Indexed Addressing mode disabled (Legacy mode)
#FUSES HSH                      //High speed Osc, high power 16MHz-25MHz
#FUSES NOPLLEN                  //4X HW PLL disabled, 4X PLL enabled in software
#FUSES BROWNOUT               
#FUSES PUT
#FUSES NOIESO
#FUSES NOFCMEN
#FUSES NOPROTECT

#use delay(clock = 20000000)
#use rs232(baud = 115200, parity = N, UART1, bits = 8)
#define LED PIN_C0
#define RTS PIN_C5

#include <can18F4580_mscp.c>  // Modified CAN library includes default FIFO mode, timing settings match MPPT, 11-bit instead of 24-bit addressing

// CAN receive buffer 0 interrupt
#int_canrx0
void isr_canrx0()
{
    int32 rx_id;
    int8  rx_len;
    int8  rx_data[8];
    struct rx_stat rxstat;
    
    if (can_getd(rx_id, rx_data, rx_len, rxstat))
    {
        // A CAN command was received, set the appropriate flag
        switch(rx_id)
        {
            case 0x304:
                output_toggle(LED);
                break;
            default:
                break;
        }
    }
}

// CAN receive buffer 1 interrupt
#int_canrx1
void isr_canrx1()
{
    int32 rx_id;
    int8  rx_len;
    int8  rx_data[8];
    struct rx_stat rxstat;
    
    if (can_getd(rx_id, rx_data, rx_len, rxstat))
    {
        // A CAN command was received, set the appropriate flag
        switch(rx_id)
        {
            case 0x304:
                output_toggle(LED);
                break;
            default:
                break;
        }
    }
}

int16 ms;
#int_timer2
void isr_timer2(void)
{
    ms++; //keep a running timer interupt that increments every milli-second
}

void main()
{
    // Enable CAN receive interrupts
    clear_interrupt(INT_CANRX0);
    enable_interrupts(INT_CANRX0);
    clear_interrupt(INT_CANRX1);
    enable_interrupts(INT_CANRX1);
    
    setup_timer_2(T2_DIV_BY_4,79,16);   //setup up timer2 to interrupt every 1ms if using 20Mhz clock 

    can_init();
    set_tris_b((*0xF93 & 0xFB ) | 0x08);   //b3 is out, b2 is in (default)
    
    //enable_interrupts(INT_CANRX1);   //enable CAN FIFO receive interrupt
    enable_interrupts(INT_TIMER2);   //enable timer2 interrupt (if want to count ms)
    enable_interrupts(GLOBAL);       //enable all interrupts
    
    output_low(LED);
    while(TRUE)
    {
    }
}
