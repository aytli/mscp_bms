/*
 the purpose of this code is to acquire serial packets from a pc, convert and send it via CANBus and vice versa
 *in the working*
 
the following is available at the CCS Index:

can_init(void);
 Initializes the CAN module to 125k baud and clears all the filters and masks so that all messages can be received from any ID.
can_set_baud(void);
 Initializes the baud rate of the CAN bus to 125kHz. It is called inside the can_init() function so there is no need to call it.
can_set_mode(CAN_OP_MODE mode);
 Allows the mode of the CAN module to be changed to configuration mode, listen mode, loop back mode, disabled mode, or normal mode.
can_set_functional_mode(CAN_FUN_OP_MODE mode);
 Allows the functional mode of ECAN modules to be changed to legacy mode, enhanced legacy mode, or first in firstout (fifo) mode. ECAN
can_set_id(int* addr, int32 id, int1 ext);
 Can be used to set the filter and mask ID's to the value specified by addr. It is also used to set the ID of the message to be sent.
can_get_id(int * addr, int1 ext);
 Returns the ID of a received message.
can_putd(int32 id, int * data, int len,int priority, int1 ext, int1 rtr);
 Constructs a CAN packet using the given arguments and places it in one of the available transmit buffers.
can_getd(int32 & id, int * data, int & len,struct rx_stat & stat);
 Retrieves a received message from one of the CAN buffers  and stores the relevant data in the referenced function parameters. 
can_enable_rtr(PROG_BUFFER b);
 Enables the automatic response feature which automatically sends a user created packet when a specified ID is received. ECAN
can_disable_rtr(PROG_BUFFER b);
 Disables the automatic response feature. ECAN 
can_load_rtr(PROG_BUFFER b, int * data, int len);
 Creates and loads the packet that will automatically transmitted when the triggering ID is received. ECAN
can_enable_filter(long filter);
 Enables one of the extra filters included in the ECAN module. ECAN
can_disable_filter(long filter);
 Disables one of the extra filters included in the ECAN module. ECAN
can_associate_filter_to_buffer(CAN_FILTER_ASSOCIATION_BUFFERS buffer,CAN_FILTER_ASSOCIATION filter);
 Used to associate a filter to a specific buffer. This allows only specific buffers to be filtered and is available in the ECAN module. ECAN
can_associate_filter_to_mask(CAN_MASK_FILTER_ASSOCIATE mask,CAN_FILTER_ASSOCIATION filter);
 Used to associate a mask to a specific buffer. This allows only specific buffer to have this mask applied. This feature is available in the ECAN module. ECAN
can_fifo_getd(int32 & id,int * data,int &len,struct rx_stat & stat);
 Retrieves the next buffer in the fifo buffer. Only available in the ECON module while operating in fifo mode. ECAN
 
Relevant Interrupts:
  
 
 #int_canirx  
 This interrupt is triggered when an invalid packet is received on the CAN.
 
#int_canwake
 This interrupt is triggered when the PIC is woken up by activity on the CAN.
 
#int_canerr
 This interrupt is triggered when there is an error in the CAN module.
 
#int_cantx0
 This interrupt is triggered when transmission from buffer 0 has completed.
 
#int_cantx1
 This interrupt is triggered when transmission from buffer 1 has completed.
 
#int_cantx2
 This interrupt is triggered when transmission from buffer 2 has completed.
 
#int_canrx0
 This interrupt is triggered when a message is received in buffer 0.
 
#int_canrx1
 This interrupt is triggered when a message is received in buffer 1. 
 
Relevant Include Files:  
 
can-mcp2510.c
 Drivers for the MCP2510 and MCP2515 interface chips
 
can-18xxx8.c
 Drivers for the built in CAN module
 
can-18F4580.c
 Drivers for the build in ECAN module
 
Example Code:

can_init();
 // initializes the CAN bus
can_putd(0x300,data,8,3,TRUE,FALSE);
 // places a message on the CAN buss with
 // ID = 0x300 and eight bytes of data pointed to by
 // “data”, the TRUE creates an extended ID, the
 // FALSE creates
can_getd(ID,data,len,stat);
 // retrieves a message from the CAN bus storing the
 // ID in the ID variable, the data at the array pointed to by
 // “data', the number of data bytes in len, and statistics
 // about the data in the stat structure.
 
 ////////////////////////////////////////////////////////////////////////
//
// can_putd()
//
// Puts data on a transmit buffer, at which time the CAN peripheral will
// send when the CAN bus becomes available.
//
//    Paramaters:
//       id - ID to transmit data as
//          enumerated as - RXB0ID,RXB1ID,B0ID,B1ID,B2ID,B3ID,B4ID,B5ID
//       data - pointer to data to send
//       len - length of data to send
//       priority - priority of message.  The higher the number, the
//                  sooner the CAN peripheral will send the message.
//                  Numbers 0 through 3 are valid.
//       ext - TRUE to use an extended ID, FALSE if not
//       rtr - TRUE to set the RTR (request) bit in the ID, false if NOT
//
//    Returns:
//       If successful, it will return TRUE
//       If un-successful, will return FALSE
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// can_getd()
//
// Gets data from a receive buffer, if the data exists
//
//    Parameters:
//      id - ID who sent message
//      data - pointer to array of data
//      len - length of received data
//      stat - structure holding some information (such as which buffer
//             recieved it, ext or standard, etc)
//
//    Returns:
//      Function call returns a TRUE if there was data in a RX buffer, FALSE
//      if there was none.
//
////////////////////////////////////////////////////////////////////////
 
*/

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
#use rs232(baud = 115200, parity = N, UART1, bits = 8)
#define LED PIN_C0
#define RTS PIN_C5

#include <can18F4580_mscp.c>  // Modified CAN library includes default FIFO mode, timing settings match MPPT, 11-bit instead of 24-bit addressing

int16 ms;

#int_timer2
void isr_timer2(void) {
   ms++; //keep a running timer interupt that increments every milli-second
}
//int bintodec(int value);
void main() {
   
   //Use local receive structure for polling receive
   struct rx_stat rxstat;
   int32 rx_id;
   int in_data[8];
   int rx_len;
   
   //Send a single packet from listed tx_id
   int out_data[8] = {0,1,2,3,4,5,6,7};
   int32 tx_id = 0x402;
   int1 tx_rtr = 0;
   int1 tx_ext = 0;
   int tx_len = 8;
   int tx_pri = 3;

   int i;
   int counter = 0;
   
   // data fields have 8 integer slots, each slot is a byte for a total of 8 bytes,
   // the PUTD and GETD function will automatically convert each integer to binary
   // prior to sending it via the CAN frame, the recieving end will also decode
   // the binary value back to an integer prior to data processing
   // max integer value per byte => 2**8 = 256 resulting in any integer number from 0...255
   
   //in_data = {0,100,200,3,4,5,6,7}; 
   //out_data = {0,0,0,3,4,5,6,7};

   setup_timer_2(T2_DIV_BY_4,79,16);   //setup up timer2 to interrupt every 1ms if using 20Mhz clock

   can_init();
   set_tris_b((*0xF93 & 0xFB ) | 0x08);   //b3 is out, b2 is in (default)
   //TRISB = 00100000; // b2 = input, b3 = output, replacement for live above
   //set_tris_c((*0xF94 & 0xBF) | 0x80);  //c6 is out, c7 is in (if using #FUSES CANC)
   //can_set_mode(CAN_OP_LOOPBACK); //Only use for loopback testing
   
   //enable_interrupts(INT_CANRX1);   //enable CAN FIFO receive interrupt
   enable_interrupts(INT_TIMER2);   //enable timer2 interrupt (if want to count ms)
   enable_interrupts(GLOBAL);       //enable all interrupts
   
   while(TRUE)
   {
      //every two seconds, send new data if transmit buffer is empty
      if ( can_tbe() && (ms > 50) )
      {
         // this will increment the first byte in the data frame every transmission
         out_data[0]++;    // increment the value of the first byte, it will return to 0 after overflow
         
         ms=0; // resets the timer interupt
         printf("\r\n\n");
         i=can_putd(tx_id,out_data,tx_len,tx_pri,tx_ext,tx_rtr); //put data on transmit buffer
         if (i != 0xFF) 
         { //if i != 0xFF then success, a transmit buffer was open
            output_toggle(LED);
            printf("\r\nSENT %U: ID=%3LX LEN=%U ", i, tx_id, tx_len);
            printf("PRI=%U EXT=%U RTR=%U\r\n   DATA = ", tx_pri, tx_ext, tx_rtr);
            for (i=0;i<tx_len;i++) 
            {
               printf("%X ",out_data[i]);
            }
            printf("\r\n");
         }
         else 
         { //fail, no transmit buffer was open
            printf("\r\nFAIL on can_putd\r\n");
         }
      }
      
      
   }  // while(true) last bracket
}     // main() last bracket

