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
//#FUSES CANC  //Enable to move CAN pins to C6(TX) and C7(RX) 

#use delay(clock = 20000000)
#use rs232(baud = 115200, parity = N, UART1, bits = 8, ERRORS)
//#use RS232(DEBUGGER)
#define LED PIN_C0
#define RTS PIN_C5

#include <can-18F4580_mscp.c>  // Modified CAN library includes default FIFO mode, timing settings match MPPT, 11-bit instead of 24-bit addressing

void main() {
   
   //Use local structure for USB/rs232 recieve
   char ch;
   char string[64];
   
   //Use local receive structure for CAN polling receive
   struct rx_stat rxstat;
   int32 rx_id;
   int in_data[8];
   int rx_len;

   int i;
   int counter = 0;

   can_init();
   set_tris_b((*0xF93 & 0xFB ) | 0x08);   //b3 is out, b2 is in (default)
   enable_interrupts(INT_TIMER2);   //enable timer2 interrupt (if want to count ms)
   enable_interrupts(GLOBAL);       //enable all interrupts
   delay_us(200);
   
   while(1)
   {
      output_high(RTS);

      // This is the polling receive routine (works)
      if (can_kbhit())   //if data is waiting in buffer...
      {
         if (can_getd(rx_id, in_data, rx_len, rxstat)) 
         {
            printf("\r\nRECIEVED: BUFF=%U ID=%3LX LEN=%U OVF=%U ",
                   rxstat.buffer,
                   rx_id,
                   rx_len,
                   rxstat.err_ovfl);
                   
            printf("FILT=%U RTR=%U EXT=%U INV=%U",
                   rxstat.filthit,
                   rxstat.rtr,
                   rxstat.ext,
                   rxstat.inv);
                   
            printf("\r\n    DATA = ");
            
            for (i = 0 ; i < rx_len ; i++)
            {
               printf("%X ",in_data[i]);
            }
            printf("\r\n");
         }
         else
         {
            printf("\r\nFAIL on can_getd\r\n");
         }
      }
   }
}

