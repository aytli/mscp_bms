////////////////////////////////////////////////////////////////////////////////
////                        can-PIC24.c                                     ////
//// CAN Library routines for Microchip's PIC24 and dsPIC33 line            ////
////                                                                        ////
//// This library provides the following functions:                         ////
////  (for more information on these functions see the comment              ////
////   header above each function)                                          ////
////                                                                        ////
////  can_init - Configures the PIC24 CAN1 peripheral                       ////
////  can2_init - Configures the PIC24 CAN2 peripheral                      ////
////                                                                        ////
////  can_set_baud - Sets the baud rate control registers for CAN1          ////
////  can2_set_baud - Sets the baud rate control registers for CAN2         ////
////                                                                        ////
////  can_set_mode - Sets the CAN1 module into a specific mode              ////
////  can2_set_mode - Sets the CAN2 module inot a specific mode             //// 
////                                                                        ////
////  can_set_id - Sets the standard and extended ID of filters and         ////
////     masks for CAN1                                                     ////
////  can2_set_id - Sets the standard and extended ID of filters and        ////
////     masks for CAN2                                                     ////
////                                                                        ////
////  can_set_buffer_id Sets the standard and extended ID of specified      ////
////     buffer for CAN1                                                    ////
////  can2_set_buffer_id Sets the standard and extended ID of specified     ////
////     buffer for CAN2                                                    ////
////                                                                        ////
////  can_set_buffer_size() Sets the DMA buffer size that CAN1 uses         ////
////  can2_set_buffer_size() Sets the DMA buffer size that CAN2 uses        ////
////                                                                        ////
////  can_get_id - Gets the standard and extended ID for CAN1               ////
////  can2_get_id - Gets the standard and extended ID for CAN2              ////
////                                                                        ////
////  can_putd - Sends a message/request with specified ID for CAN1         ////
////  can2_putd - Sends a message/request with specified ID for CAN2        ////
////                                                                        ////
////  can_getd - Returns specifid message/request and ID for CAN1           ////
////  can2_getd - Returns specifid message/request and ID for CAN2          ////
////                                                                        ////
////  can_kbhit - Returns true if there is data in one of the receive       ////
////     buffers for CAN1                                                   ////
////  can2_kbhit - Returns true if there is data in one of the receive      ////
////     buffers for CAN2                                                   ////
////                                                                        ////
////  can_tbe - Returns true if the transmit buffer is ready to send        ////
////     more data for CAN1                                                 ////
////  can2_tbe - Returns true if the transmit buffer is ready to send       ////
////     more data for CAN2                                                 ////
////                                                                        ////
////  can_abort - Aborts all pending transmissions for CAN1                 ////
////  can2_abort - Aborts all pending transmissions for CAN2                ////
////                                                                        ////
////  can_enable_b_transfer - enables buffer as transmitter for CAN1        ////
////  can2_enable_b_transfer - enables buffer as transmitter for CAN2       ////
////                                                                        ////
////  can_enable_b_receiver - enables buffer as receiver for CAN1           ////
////  can2_enable_b_receiver - enables buffer as receiver for CAN2          ////
////                                                                        ////
////  can_enable_rtr - enables transmit buffer as Auto-rtr for CAN1         ////
////  can2_enable_rtr - enables transmit buffer as Auto-rtr for CAN2        ////
////                                                                        ////
////  can_disable_rtr - disables transmit buffer as Auto-rtr for CAN1       ////
////  can2_disable_rtr - disables transmit buffer as Auto-rtr for CAN2      ////
////                                                                        ////
////  can_load_rtr - loads an Auto-rtr buffer with data for CAN1            ////
////  can2_load_rtr - loads an Auto-rtr buffer with data for CAN2           ////
////                                                                        ////
////  can_enable_filter - enables one of 15 filters for CAN1                ////
////  can2_enable_filter - enables one of 15 filters for CAN2               ////
////                                                                        ////
////  can_disable_filter - disables one of the 15 filters for CAN1          ////
////  can2_disable_filter - disables one of the 15 filters for CAN2         ////
////                                                                        ////
////  can_associate_filter_to_buffer - associates a filter with a           ////
////     buffer for CAN1                                                    ////
////  can2_associate_filter_to_buffer - associates a filter with a          ////
////     buffer for CAN2                                                    ////
////                                                                        ////
////  can_associate_filter_to_mask - associates a filter with a mask        ////
////     for CAN1                                                           ////
////  can2_associate_filter_to_mask - associates a filter with a mask       ////
////     for CAN2                                                           ////
////                                                                        ////
////  can_fifo_getd - retrive data in FIFO mode for CAN1                    ////
////  can2_fifo_getd - retrive data in FIFO mode for CAN2                   ////
////                                                                        ////
////  can_trb0_putd                                                         ////
////  can_trb1_putd                                                         ////
////  can_trb2_putd                                                         ////
////  can_trb3_putd                                                         ////
////  can_trb4_putd                                                         ////
////  can_trb5_putd                                                         ////
////  can_trb6_putd                                                         ////
////  can_trb7_putd - place data in a specific buffer for CAN1              ////
////  can2_trb0_putd                                                        ////
////  can2_trb1_putd                                                        ////
////  can2_trb2_putd                                                        ////
////  can2_trb3_putd                                                        ////
////  can2_trb4_putd                                                        ////
////  can2_trb5_putd                                                        ////
////  can2_trb6_putd                                                        ////
////  can2_trb7_putd - place data in a specific buffer for CAN2             ////
////                                                                        ////
////  can_enable_interrupts - enables specified interrupts for #INT_CAN1    ////
////  can2_enable_interrupts - enables specified interrupts for #INT_CAN2   ////
////                                                                        ////
////  can_disable_interrupts - disables specified interrupts for #INT_CAN1  ////
////  can2_disable_interrupts - disables specified interrupts for #INT_CAN2 ////
////                                                                        ////
////  can_config_DMA - configures DMA buffers for CAN1                      ////
////  can2_config_DMA - configures DMA buffers for CAN2                     ////
////                                                                        ////
//// PIN_F0 is C1RX, PIN_F1 is C1TX, PIN_G0 is C2RX and PIN_G1 is C2TX. You ////
//// will need a CAN transeiver to connect these pins to CANH and CANL bus  ////
//// lines.                                                                 ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2010 Custom Computer Services                ////
//// This source code may only be used by licensed users of the CCS         ////
//// C compiler.  This source code may only be distributed to other         ////
//// licensed users of the CCS C compiler.  No other use,                   ////
//// reproduction or distribution is permitted without written              ////
//// permission.  Derivative programs created using this software           ////
//// in object code form are not restricted in any way.                     ////
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <can_PIC24.h>

#if CAN_DO_DEBUG
 #define can_debug printf
#else
 #define can_debug
#endif

//macros
#define can_kbhit() (C1RXFUL1.rxful0 || C1RXFUL1.rxful1 || C1RXFUL1.rxful2 || C1RXFUL1.rxful3 || C1RXFUL1.rxful4 || C1RXFUL1.rxful5 || C1RXFUL1.rxful6 || C1RXFUL1.rxful7 || C1RXFUL1.rxful8 || C1RXFUL1.rxful9 || C1RXFUL1.rxful10 || C1RXFUL1.rxful11 || C1RXFUL1.rxful12 || C1RXFUL1.rxful13 || C1RXFUL1.rxful14 || C1RXFUL1.rxful15 || C1RXFUL2.rxful16 || C1RXFUL2.rxful17 || C1RXFUL2.rxful18 || C1RXFUL2.rxful19 || C1RXFUL2.rxful20 || C1RXFUL2.rxful21 || C1RXFUL2.rxful22 || C1RXFUL2.rxful23 || C1RXFUL2.rxful24 || C1RXFUL2.rxful25 || C1RXFUL2.rxful26 || C1RXFUL2.rxful27 || C1RXFUL2.rxful28 || C1RXFUL2.rxful29 || C1RXFUL2.rxful30 || C1RXFUL2.rxful31)
#define can_tbe() ((C1TR01CON.txenm && !C1TR01CON.txreqm) || (C1TR01CON.txenn && !C1TR01CON.txreqn) || (C1TR23CON.txenm && !C1TR23CON.txreqm) || (C1TR23CON.txenn && !C1TR23CON.txreqn) || (C1TR45CON.txenm && !C1TR45CON.txreqm) || (C1TR45CON.txenn && !C1TR45CON.txreqn) || ( C1TR67CON.txenm && !C1TR67CON.txreqm) || (C1TR67CON.txenn && !C1TR67CON.txreqn))
#define can_abort()                 (C1CTRL1.abat=1)

#if getenv("SFR:C2CTRL1")
   #define can2_kbhit() (C2RXFUL1.rxful0 || C2RXFUL1.rxful1 || C2RXFUL1.rxful2 || C2RXFUL1.rxful3 || C2RXFUL1.rxful4 || C2RXFUL1.rxful5 || C2RXFUL1.rxful6 || C2RXFUL1.rxful7 || C2RXFUL1.rxful8 || C2RXFUL1.rxful9 || C2RXFUL1.rxful10 || C2RXFUL1.rxful11 || C2RXFUL1.rxful12 || C2RXFUL1.rxful13 || C2RXFUL1.rxful14 || C2RXFUL1.rxful15 || C2RXFUL2.rxful16 || C2RXFUL2.rxful17 || C2RXFUL2.rxful18 || C2RXFUL2.rxful19 || C2RXFUL2.rxful20 || C2RXFUL2.rxful21 || C2RXFUL2.rxful22 || C2RXFUL2.rxful23 || C2RXFUL2.rxful24 || C2RXFUL2.rxful25 || C2RXFUL2.rxful26 || C2RXFUL2.rxful27 || C2RXFUL2.rxful28 || C2RXFUL2.rxful29 || C2RXFUL2.rxful30 || C2RXFUL2.rxful31)
   #define can2_tbe() ((C2TR01CON.txenm && !C2TR01CON.txreqm) || (C2TR01CON.txenn && !C2TR01CON.txreqn) || (C2TR23CON.txenm && !C2TR23CON.txreqm) || (C2TR23CON.txenn && !C2TR23CON.txreqn) || (C2TR45CON.txenm && !C2TR45CON.txreqm) || (C2TR45CON.txenn && !C2TR45CON.txreqn) || ( C2TR67CON.txenm && !C2TR67CON.txreqm) || (C2TR67CON.txenn && !C2TR67CON.txreqn))
   #define can2_abort()                 (C2CTRL1.abat=1)
#endif

// current mode variable
// used by many of the device drivers to prevent damage from the mode
CAN_OP_MODE curmode;

// Buffer for CAN1
#if CAN_BUFFER_SIZE==4 
   #BANK_DMA
   uint16_t ecan1_message_buffer[4][8];
#elif CAN_BUFFER_SIZE==6
   #BANK_DMA
   uint16_t ecan1_message_buffer[6][8];
#elif CAN_BUFFER_SIZE==8
   #BANK_DMA
   uint16_t ecan1_message_buffer[8][8];
#elif CAN_BUFFER_SIZE==12
   #BANK_DMA
   uint16_t ecan1_message_buffer[12][8];
#elif CAN_BUFFER_SIZE==16
   #BANK_DMA
   uint16_t ecan1_message_buffer[16][8];
#elif CAN_BUFFER_SIZE==24
   #BANK_DMA
   uint16_t ecan1_message_buffer[24][8];
#else
   #BANK_DMA
   uint16_t ecan1_message_buffer[32][8];
#endif

// Buffer for CAN2
#if getenv("SFR:C2CTRL1")
   #if CAN2_BUFFER_SIZE==4 
      #BANK_DMA
      uint16_t ecan2_message_buffer[4][8];
   #elif CAN2_BUFFER_SIZE==6
      #BANK_DMA
      uint16_t ecan2_message_buffer[6][8];
   #elif CAN2_BUFFER_SIZE==8
      #BANK_DMA
      uint16_t ecan2_message_buffer[8][8];
   #elif CAN2_BUFFER_SIZE==12
      #BANK_DMA
      uint16_t ecan2_message_buffer[12][8];
   #elif CAN2_BUFFER_SIZE==16
      #BANK_DMA
      uint16_t ecan2_message_buffer[16][8];
   #elif CAN2_BUFFER_SIZE==24
      #BANK_DMA
      uint16_t ecan2_message_buffer[24][8];
   #else
      #BANK_DMA
      uint16_t ecan2_message_buffer[32][8];
   #endif
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_init()
// can2_init()
//
// Initializes PIC24 CAN peripheral.  Sets the RX filter and masks so the
// CAN peripheral will receive all incoming IDs. Enables Filter 0 and associates
// it with the FIFO buffer.
//
// The constant (CAN_ENABLE_CAN_CAPTURE) are given a default define in the 
// can-PIC24.h file.  These default values can be overwritten in the main code, 
// but most applications will be fine with these defaults.
//
////////////////////////////////////////////////////////////////////////////////
void can_init(void) {
   can_set_mode(CAN_OP_CONFIG);   //must be in config mode before params can be set
   
   C1CTRL1.cancap=CAN_ENABLE_CAN_CAPTURE;
   C1CTRL1.cancks=0;
   can_set_baud();
   
   can_set_buffer_size(CAN_BUFFER_SIZE);      // sets size of DMA buffer to use
   can_config_DMA();   
      
   can_set_id(&C1RXM0, CAN_MASK_ACCEPT_ALL, CAN_USE_EXTENDED_ID);  //set mask 0
   can_set_id(&C1RXM1, CAN_MASK_ACCEPT_ALL, CAN_USE_EXTENDED_ID);  //set mask 1
   can_set_id(&C1RXM2, CAN_MASK_ACCEPT_ALL, CAN_USE_EXTENDED_ID);  //set mask 2
   
   // set dynamic filters
   can_set_id(&C1RXF0, 0, CAN_USE_EXTENDED_ID); 
   can_set_id(&C1RXF1, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF2, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF3, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF4, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF5, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF6, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF7, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF8, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF9, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF10, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF11, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF12, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF13, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF14, 0, CAN_USE_EXTENDED_ID);
   can_set_id(&C1RXF15, 0, CAN_USE_EXTENDED_ID);
   
   C1FEN1=0;
   
   can_associate_filter_to_buffer(AFIFO,F0BP);
   can_enable_filter(FLTEN0);
   
   can_set_mode(CAN_OP_NORMAL);
}

#if getenv("SFR:C2CTRL1")
   void can2_init(void) {
      can2_set_mode(CAN_OP_CONFIG);   //must be in config mode before params can be set
      
      C2CTRL1.cancap=CAN_ENABLE_CAN_CAPTURE;
      C2CTRL1.cancks=0;
      can2_set_baud();
      
      can2_set_buffer_size(CAN2_BUFFER_SIZE);      // sets size of DMA buffer to use
      can2_config_DMA();   
         
      can2_set_id(&C2RXM0, CAN_MASK_ACCEPT_ALL, CAN_USE_EXTENDED_ID);  //set mask 0
      can2_set_id(&C2RXM1, CAN_MASK_ACCEPT_ALL, CAN_USE_EXTENDED_ID);  //set mask 1
      can2_set_id(&C2RXM2, CAN_MASK_ACCEPT_ALL, CAN_USE_EXTENDED_ID);  //set mask 2
      
      // set dynamic filters
      can2_set_id(&C2RXF0, 0, CAN_USE_EXTENDED_ID); 
      can2_set_id(&C2RXF1, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF2, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF3, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF4, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF5, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF6, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF7, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF8, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF9, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF10, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF11, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF12, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF13, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF14, 0, CAN_USE_EXTENDED_ID);
      can2_set_id(&C2RXF15, 0, CAN_USE_EXTENDED_ID);
      
      C2FEN1=0;
      
      can2_associate_filter_to_buffer(AFIFO,F0BP);
      can2_enable_filter(FLTEN0);
      
      can2_set_mode(CAN_OP_NORMAL);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_set_baud()
// can2_set_baud()
//
// Configures the baud rate control registers.  All the defines here
// are defaulted in the can-PIC24.h file.  These defaults can, and
// probably should, be overwritten in the main code.
//
// Current defaults are set to work with Microchip's MCP250xxx CAN
// Developers Kit if this PIC is running at 20Mhz.
//
////////////////////////////////////////////////////////////////////////////////
void can_set_baud(void) {
   C1CFG1.brp=CAN_BRG_PRESCALAR;
   C1CFG1.sjw=CAN_BRG_SYNCH_JUMP_WIDTH;

   C1CFG2.prseg=CAN_BRG_PROPAGATION_TIME;
   C1CFG2.seg1ph=CAN_BRG_PHASE_SEGMENT_1;
   C1CFG2.sam=CAN_BRG_SAM;
   C1CFG2.seg2phts=CAN_BRG_SEG_2_PHASE_TS;
   C1CFG2.seg2ph=CAN_BRG_PHASE_SEGMENT_2;
   C1CFG2.wakfil=CAN_BRG_WAKE_FILTER;
}

#if getenv("SFR:C2CTRL1")
   void can2_set_baud(void) {
      C2CFG1.brp=CAN2_BRG_PRESCALAR;
      C2CFG1.sjw=CAN2_BRG_SYNCH_JUMP_WIDTH;
   
      C2CFG2.prseg=CAN2_BRG_PROPAGATION_TIME;
      C2CFG2.seg1ph=CAN2_BRG_PHASE_SEGMENT_1;
      C2CFG2.sam=CAN2_BRG_SAM;
      C2CFG2.seg2phts=CAN2_BRG_SEG_2_PHASE_TS;
      C2CFG2.seg2ph=CAN2_BRG_PHASE_SEGMENT_2;
      C2CFG2.wakfil=CAN2_BRG_WAKE_FILTER;
   }
#endif
   
////////////////////////////////////////////////////////////////////////////////
//
// can_set_mode()
// can2_set_mode()
//
// This function first sets the three bits (reqop2:reqop0) to the mode given 
// through the argument.
//
// The reqop bits do not immediatly change the mode of operation, the
// opmode bits in the C1CTRL1 or C2CTRL1 register (opmode2:opmode0) must change 
// to reflect the actuall change in mode, therefore a while statement is 
// used to check if the C1CTRL1 or C2CTRL1 opmode bits have changed to reflect 
// the change in mode.
//
////////////////////////////////////////////////////////////////////////////////
void can_set_mode(CAN_OP_MODE mode) {
   C1CTRL1.reqop=mode;
   while( (C1CTRL1.opmode) != mode );
}

#if getenv("SFR:C2CTRL1")
   void can2_set_mode(CAN_OP_MODE mode) {
      C2CTRL1.reqop=mode;
      while( (C2CTRL1.opmode) != mode );
   }
#endif
   
////////////////////////////////////////////////////////////////////////////////
//
// can_set_id()
// can2_set_id()
//
// Configures the xxxxEID and xxxxSID registers to configure the defined 
// acceptence filter or acceptance filter mask to the specified ID
//
//   Paramaters:
//     addr - pointer to first byte of ID register, starting with xxxxEID.
//            For example, a pointer to C1RXF0
//
//     id - ID to set filter or mask to
//     ext - Set to TRUE if this filter or mask uses an extended ID, 
//     FALSE if not
//
////////////////////////////////////////////////////////////////////////////////
void can_set_id(uint16_t *addr, uint32_t id, int1 ext) 
{
   uint16_t temp;
   curmode=C1CTRL1.opmode;

   can_set_mode(CAN_OP_CONFIG);
   
   C1CTRL1.win=1;
   
   if (ext) {  //extended
      //eid
      *addr=(id & 0xffff);                         //0:15
   
      //sid
      addr-=1;
      *addr&=0x1C;
      temp=((id & 0x30000) >> 16);
      temp|=((id & 0x1FFC0000) >> 13);
      *addr|=temp;                                 //16:28
   }
      
   else {   //standard
      //eid
      *addr=0;

      //sid
      addr-=1;
      *addr&=0x1C;
      temp=((id & 0x7FF) << 5);
      *addr|=temp;                              //0:10
   }
   C1CTRL1.win=0;
   
   can_set_mode(curmode);
}

#if getenv("SFR:C2CTRL1")
   void can2_set_id(uint16_t *addr, uint32_t id, int1 ext) 
   {
      uint16_t temp;
      curmode=C2CTRL1.opmode;
   
      can2_set_mode(CAN_OP_CONFIG);
      
      C2CTRL1.win=1;
      
      if (ext) {  //extended
         //eid
         *addr=(id & 0xffff);                         //0:15
      
         //sid
         addr-=1;
         *addr&=0x1C;
         temp=((id & 0x30000) >> 16);
         temp|=((id & 0x1FFC0000) >> 13);
         *addr|=temp;                                 //16:28
      }
         
      else {   //standard
         //eid
         *addr=0;
   
         //sid
         addr-=1;
         *addr&=0x1C;
         temp=((id & 0x7FF) << 5);
         *addr|=temp;                              //0:10
      }
      C2CTRL1.win=0;
      
      can2_set_mode(curmode);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_set_buffer_id()
// can2_set_buffer_id()
//
// Configures the defined buffer to use the specifed ID
//
// Parameters:
//       buffer - the desired buffer
//        emumberated as TRB0-TRB7
//
//       id - ID to set buffer to
//       ext - Set to TRUE is this buffer uses an extended ID, False if not
//
////////////////////////////////////////////////////////////////////////////////
void can_set_buffer_id(BUFFER buffer, uint32_t id, int1 ext)
{
   uint16_t *ptr;
   uint16_t temp;
   
   ptr=&ecan1_message_buffer[buffer][2];

   if (ext)
   {
      //eid
      *ptr&=0x20F;
      temp=((id & 0x3F) << 10);
      *ptr|=temp;
      ptr-=1;
      temp=((id & 0x3FFC0) >> 6);
      *ptr=temp;
      
      //sid
      ptr-=1;
      *ptr&=0x2;
      temp=(((id & 0x1FFC0000) >> 16) | ext);
      *ptr|=temp;
   }
   else
   {
      //eid
      *ptr&=0x20F;
      ptr-=1;
      *ptr=0;
     
      //sid
      ptr-=1;
      *ptr&=0x2;
      temp=(((id & 0x7FF) << 2) | ext);
      *ptr|=temp;
   }
}

#if getenv("SFR:C2CTRL1")
   void can2_set_buffer_id(BUFFER buffer, uint32_t id, int1 ext)
   {
      uint16_t *ptr;
      uint16_t temp;
      
      ptr=&ecan2_message_buffer[buffer][2];
   
      if (ext)
      {
         //eid
         *ptr&=0x20F;
         temp=((id & 0x3F) << 10);
         *ptr|=temp;
         ptr-=1;
         temp=((id & 0x3FFC0) >> 6);
         *ptr=temp;
         
         //sid
         ptr-=1;
         *ptr&=0x2;
         temp=(((id & 0x1FFC0000) >> 16) | ext);
         *ptr|=temp;
      }
      else
      {
         //eid
         *ptr&=0x20F;
         ptr-=1;
         *ptr=0;
        
         //sid
         ptr-=1;
         *ptr&=0x2;
         temp=(((id & 0x7FF) << 2) | ext);
         *ptr|=temp;
      }
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_set_buffer_size()
// can2_set_buffer_size()
//
// Configures the number of DMA buffers the ECAN module uses.
//
// Parameters:
//       size - the desired buffer size to be used. Valid sizes 4, 6, 8, 12, 16,
//              24, and 32
//
// Returns:
//       void
//
////////////////////////////////////////////////////////////////////////////////
void can_set_buffer_size(uint8_t size)
{
   if (size==4)
      C1FCTRL.dmabs=0;
   else if (size==6)
      C1FCTRL.dmabs=1;
   else if (size==8)
      C1FCTRL.dmabs=2;
   else if (size==12)
      C1FCTRL.dmabs=3;
   else if (size==16)
      C1FCTRL.dmabs=4;
   else if (size==24)
      C1FCTRL.dmabs=5;
   else
      C1FCTRL.dmabs=6;
}

#if getenv("SFR:C2CTRL1")
   void can2_set_buffer_size(uint8_t size)
   {
      if (size==4)
         C2FCTRL.dmabs=0;
      else if (size==6)
         C2FCTRL.dmabs=1;
      else if (size==8)
         C2FCTRL.dmabs=2;
      else if (size==12)
         C2FCTRL.dmabs=3;
      else if (size==16)
         C2FCTRL.dmabs=4;
      else if (size==24)
         C2FCTRL.dmabs=5;
      else
         C2FCTRL.dmabs=6;
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_enable_filter()
// can2_enable_filter()
//
// Enables a given acceptance filter
//
// Parameters:
//      filter - the filter that is to be enabled
//         enumerated as - FLTEN0-FLTEN15
//
// Returns:
//    void
//
// More information can be found on Acceptance Filters in the PIC24H Family 
// Reference Manual, Section 21
//
////////////////////////////////////////////////////////////////////////////////
void can_enable_filter(CAN_FILTER_CONTROL filter)
{
   uint16_t *ptr;

   curmode=C1CTRL1.opmode;

   can_set_mode(CAN_OP_CONFIG);

   ptr = &C1FEN1;

   *ptr|=filter;

   can_set_mode(curmode);
}

#if getenv("SFR:C2CTRL1")
   void can2_enable_filter(CAN_FILTER_CONTROL filter)
   {
      uint16_t *ptr;
   
      curmode=C2CTRL1.opmode;
   
      can2_set_mode(CAN_OP_CONFIG);
   
      ptr = &C2FEN1;
   
      *ptr|=filter;
   
      can2_set_mode(curmode);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_disable_filter()
// can2_disable_filter()
//
// Disables a given acceptance filter
//
// Parameters:
//      filter - the filter that is to be disabled
//        enumerated as - FLTEN0-FLTEN15
//
// Returns:
//      void;
//
// More information can be found on Acceptance Filters in the PIC24H Family 
// Reference Manual, Section 21
//
////////////////////////////////////////////////////////////////////////////////
void can_disable_filter(CAN_FILTER_CONTROL filter)
{
   uint16_t *ptr;

   curmode=C1CTRL1.opmode;

   can_set_mode(CAN_OP_CONFIG);

   ptr = &C1FEN1;

   *ptr&=~filter;

   can_set_mode(curmode);
}

#if getenv("SFR:C2CTRL1")
   void can2_disable_filter(CAN_FILTER_CONTROL filter)
   {
      uint16_t *ptr;
   
      curmode=C2CTRL1.opmode;
   
      can2_set_mode(CAN_OP_CONFIG);
   
      ptr = &C2FEN1;
   
      *ptr&=~filter;
   
      can2_set_mode(curmode);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_associate_filter_to_buffer()
// can2_associate_filter_to_buffer()
//
// Associates a given buffer with a given filter register
//
// Parameters:
//      buffer - the buffer that is to be associated with the filter
//        enumerated as
//               ATRB0-ATRB7 - Programmable receive Buffers 0 - 7
//               ARB8-ARB14 - Dedicated receive buffers 8-14
//               AFIFO - FIFO buffer
//
//      filter - the filter that is to be associated with the buffer
//         enumerated as
//               F0BP-F15BP - Filters 0 - 15
//
// Returns:
//      void
// 
////////////////////////////////////////////////////////////////////////////////
void can_associate_filter_to_buffer(CAN_FILTER_ASSOCIATION_BUFFERS buffer, CAN_FILTER_ASSOCIATION filter)
{
   curmode=C1CTRL1.opmode;

   can_set_mode(CAN_OP_CONFIG);
   
   C1CTRL1.win=1;
   
   switch(filter)
   {
      case 0:
         C1BUFPNT1.f0bp=buffer;
         break;
      case 1:
         C1BUFPNT1.f1bp=buffer;
         break;
      case 2:
         C1BUFPNT1.f2bp=buffer;
         break;
      case 3:
         C1BUFPNT1.f3bp=buffer;
         break;
      case 4:
         C1BUFPNT2.f4bp=buffer;
         break;
      case 5:
         C1BUFPNT2.f5bp=buffer;
         break;
      case 6:
         C1BUFPNT2.f6bp=buffer;
         break;
      case 7:
         C1BUFPNT2.f7bp=buffer;
         break;
      case 8:
         C1BUFPNT3.f8bp=buffer;
         break;
      case 9:
         C1BUFPNT3.f9bp=buffer;
         break;
      case 10:
         C1BUFPNT3.f10bp=buffer;
         break;
      case 11:
         C1BUFPNT3.f11bp=buffer;
         break;
      case 12:
         C1BUFPNT4.f12bp=buffer;
         break;
      case 13:
         C1BUFPNT4.f13bp=buffer;
         break;
      case 14:
         C1BUFPNT4.f14bp=buffer;
         break;
      case 15:
         C1BUFPNT4.f15bp=buffer;
         break;
   }
   C1CTRL1.win=0;
   
   can_set_mode(curmode);
}

#if getenv("SFR:C2CTRL1")
   void can2_associate_filter_to_buffer(CAN_FILTER_ASSOCIATION_BUFFERS buffer, CAN_FILTER_ASSOCIATION filter)
   {
      curmode=C2CTRL1.opmode;
   
      can2_set_mode(CAN_OP_CONFIG);
      
      C2CTRL1.win=1;
      
      switch(filter)
      {
         case 0:
            C2BUFPNT1.f0bp=buffer;
            break;
         case 1:
            C2BUFPNT1.f1bp=buffer;
            break;
         case 2:
            C2BUFPNT1.f2bp=buffer;
            break;
         case 3:
            C2BUFPNT1.f3bp=buffer;
            break;
         case 4:
            C2BUFPNT2.f4bp=buffer;
            break;
         case 5:
            C2BUFPNT2.f5bp=buffer;
            break;
         case 6:
            C2BUFPNT2.f6bp=buffer;
            break;
         case 7:
            C2BUFPNT2.f7bp=buffer;
            break;
         case 8:
            C2BUFPNT3.f8bp=buffer;
            break;
         case 9:
            C2BUFPNT3.f9bp=buffer;
            break;
         case 10:
            C2BUFPNT3.f10bp=buffer;
            break;
         case 11:
            C2BUFPNT3.f11bp=buffer;
            break;
         case 12:
            C2BUFPNT4.f12bp=buffer;
            break;
         case 13:
            C2BUFPNT4.f13bp=buffer;
            break;
         case 14:
            C2BUFPNT4.f14bp=buffer;
            break;
         case 15:
            C2BUFPNT4.f15bp=buffer;
            break;
      }
      C2CTRL1.win=0;
      
      can2_set_mode(curmode);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_associate_filter_to_mask()
// can2_associate_filter_to_mask()
//
//   Associates a given filter to a given mask
//
//   Parameters:
//      mask - the mask that is to be associated with the filter
//         enumerated as
//            ACCEPTANCE_MASK_0
//            ACCEPTANCE_MASK_1
//            ACCEPTANCE_MASK_2
//            NO_MASK
//
//      filter - the filter that is to be associated with the mask
//         enumerated as
//               F0BP-F15BP - Filters 0 - 15
//
//   Returns:
//      void
//
// More information can be found on Acceptance Filters in the PIC24H Family 
// Reference Manual, Section 21
//
////////////////////////////////////////////////////////////////////////////////
void can_associate_filter_to_mask(CAN_MASK_FILTER_ASSOCIATION mask, CAN_FILTER_ASSOCIATION filter)
{
   curmode = C1CTRL1.opmode;

   can_set_mode(CAN_OP_CONFIG);
   
   switch(filter)
   {
      case 0:
         C1FMSKSEL1.f0msk=mask;
         break;
      case 1:
         C1FMSKSEL1.f1msk=mask;
         break;
      case 2:
         C1FMSKSEL1.f2msk=mask;
         break;
      case 3:
         C1FMSKSEL1.f3msk=mask;
         break;
      case 4:
         C1FMSKSEL1.f4msk=mask;
         break;
      case 5:
         C1FMSKSEL1.f5msk=mask;
         break;
      case 6:
         C1FMSKSEL1.f6msk=mask;
         break;
      case 7:
         C1FMSKSEL1.f7msk=mask;
         break;
      case 8:
         C1FMSKSEL2.f8msk=mask;
         break;
      case 9:
         C1FMSKSEL2.f9msk=mask;
         break;
      case 10:
         C1FMSKSEL2.f10msk=mask;
         break;
      case 11:
         C1FMSKSEL2.f11msk=mask;
         break;
      case 12:
         C1FMSKSEL2.f12msk=mask;
         break;
      case 13:
         C1FMSKSEL2.f13msk=mask;
         break;
      case 14:
         C1FMSKSEL2.f14msk=mask;
         break;
      case 15:
         C1FMSKSEL2.f15msk=mask;
         break;
   }
   can_set_mode(curmode);
}

#if getenv("SFR:C2CTRL1")
   void can2_associate_filter_to_mask(CAN_MASK_FILTER_ASSOCIATION mask, CAN_FILTER_ASSOCIATION filter)
   {
      curmode = C2CTRL1.opmode;
   
      can2_set_mode(CAN_OP_CONFIG);
      
      switch(filter)
      {
         case 0:
            C2FMSKSEL1.f0msk=mask;
            break;
         case 1:
            C2FMSKSEL1.f1msk=mask;
            break;
         case 2:
            C2FMSKSEL1.f2msk=mask;
            break;
         case 3:
            C2FMSKSEL1.f3msk=mask;
            break;
         case 4:
            C2FMSKSEL1.f4msk=mask;
            break;
         case 5:
            C2FMSKSEL1.f5msk=mask;
            break;
         case 6:
            C2FMSKSEL1.f6msk=mask;
            break;
         case 7:
            C2FMSKSEL1.f7msk=mask;
            break;
         case 8:
            C2FMSKSEL2.f8msk=mask;
            break;
         case 9:
            C2FMSKSEL2.f9msk=mask;
            break;
         case 10:
            C2FMSKSEL2.f10msk=mask;
            break;
         case 11:
            C2FMSKSEL2.f11msk=mask;
            break;
         case 12:
            C2FMSKSEL2.f12msk=mask;
            break;
         case 13:
            C2FMSKSEL2.f13msk=mask;
            break;
         case 14:
            C2FMSKSEL2.f14msk=mask;
            break;
         case 15:
            C2FMSKSEL2.f15msk=mask;
            break;
      }
      can2_set_mode(curmode);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_putd()
// can2_putd()
//
// Puts data on a transmit buffer, at which time the CAN peripheral will
// send when the CAN bus becomes available.
//
//    Paramaters:
//       id - ID to transmit data as
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
////////////////////////////////////////////////////////////////////////////////
int1 can_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t priority, int1 ext, int1 rtr) 
{
   uint16_t *ptr;
   uint16_t i;
   uint16_t port;
   uint16_t temp;
      
    // find emtpy transmitter
   if (C1TR01CON.txenm && !C1TR01CON.txreqm && !C1TR01CON.rtrenm)
      port=0;
   else if (C1TR01CON.txenn && !C1TR01CON.txreqn && !C1TR01CON.rtrenn)
      port=1;
   else if (C1TR23CON.txenm && !C1TR23CON.txreqm && !C1TR23CON.rtrenm)
      port=2;
   else if (C1TR23CON.txenn && !C1TR23CON.txreqn && !C1TR23CON.rtrenn)
      port=3;
   else if (C1TR45CON.txenm && !C1TR45CON.txreqm && !C1TR45CON.rtrenm)
      port=4;
   else if (C1TR45CON.txenn && !C1TR45CON.txreqn && !C1TR45CON.rtrenn)
      port=5;
   else if (C1TR67CON.txenm && !C1TR67CON.txreqm && !C1TR67CON.rtrenm)
      port=6;
   else if (C1TR67CON.txenn && !C1TR67CON.txreqn && !C1TR67CON.rtrenn)
      port=7;
   else {
      #if CAN_DO_DEBUG
         can_debug("\r\nCAN_PUTD() FAIL: NO OPEN TX BUFFERS\r\n");
      #endif
      return(0);
   }
   
   // load data into correct buffer
   if(ext)
   {
      temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
      ecan1_message_buffer[port][0]=temp;
      temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
      ecan1_message_buffer[port][1]=temp;
      temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
      ecan1_message_buffer[port][2]=temp;
   }
   else
   {  
      temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
      ecan1_message_buffer[port][0]=temp;
      ecan1_message_buffer[port][1]=0x0000;
      temp=(((uint16_t)rtr << 9) | len) & 0xFF1F;
      ecan1_message_buffer[port][2]=temp;
   }
   
   ptr=&ecan1_message_buffer[port][3];
   for(i=0;i<len;i+=2)
   {
      if (i+1==len)
         *ptr=make16(0,data[i]);
      else
         *ptr=make16(data[i+1],data[i]);
      ptr++;
   }
   printf("\r\n%LX",ecan1_message_buffer[port][3]);
   printf("\r\n%LX",ecan1_message_buffer[port][4]);
   printf("\r\n%LX",ecan1_message_buffer[port][5]);
   printf("\r\n%LX",ecan1_message_buffer[port][6]);
   
   switch(port)
   {
      case 0:
         C1TR01CON.txmpri=priority;          //set priority DMA buffer 0
         C1TR01CON.txreqm=1;                 //enable transmission buffer 0
         break;
      case 1:
         C1TR01CON.txnpri=priority;          //set priority DMA buffer 1
         C1TR01CON.txreqn=1;                 //enable transmission buffer 1
         break;
      case 2:
         C1TR23CON.txmpri=priority;          //set priority DMA buffer 2
         C1TR23CON.txreqm=1;                 //enable transmission buffer 2
         break;
      case 3:
         C1TR23CON.txnpri=priority;          //set priority DMA buffer 3
         C1TR23CON.txreqn=1;                 //enable transmission buffer 3
         break;
      case 4:
         C1TR45CON.txmpri=priority;          //set priority DMA buffer 4
         C1TR45CON.txreqm=1;                 //enable transmission buffer 4
         break;
      case 5:
         C1TR45CON.txnpri=priority;          //set priority DMA buffer 5
         C1TR45CON.txreqn=1;                 //enable transmission buffer 5
         break;
      case 6:
         C1TR67CON.txmpri=priority;          //set priority DMA buffer 6
         C1TR67CON.txreqm=1;                 //enable transmission buffer 6
         break;
      case 7:
         C1TR67CON.txnpri=priority;          //set priority DMA buffer 7
         C1TR67CON.txreqn=1;                 //enable transmission buffer 7
         break;
   }
       
   #if CAN_DO_DEBUG
            can_debug("\r\nCAN_PUTD(): BUFF=%U ID=%LX LEN=%U PRI=%U EXT=%U RTR=%U\r\n", port, id, len, priority, ext, rtr);
            if ((len)&&(!rtr)) {
               can_debug("  DATA = ");
               for (i=0;i<len;i++) {
                  can_debug("%X ",*data);
                  data++;
               }
               can_debug("\r\n");
            }
   #endif 

   return(1);
}

#if getenv("SFR:C2CTRL1")
   int1 can2_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t priority, int1 ext, int1 rtr)
   {
      uint16_t *ptr;
      uint16_t i;
      uint16_t port;
      uint16_t temp;
         
       // find emtpy transmitter
      if (C2TR01CON.txenm && !C2TR01CON.txreqm && !C2TR01CON.rtrenm)
         port=0;
      else if (C2TR01CON.txenn && !C2TR01CON.txreqn && !C2TR01CON.rtrenn)
         port=1;
      else if (C2TR23CON.txenm && !C2TR23CON.txreqm && !C2TR23CON.rtrenm)
         port=2;
      else if (C2TR23CON.txenn && !C2TR23CON.txreqn && !C2TR23CON.rtrenn)
         port=3;
      else if (C2TR45CON.txenm && !C2TR45CON.txreqm && !C2TR45CON.rtrenm)
         port=4;
      else if (C2TR45CON.txenn && !C2TR45CON.txreqn && !C2TR45CON.rtrenn)
         port=5;
      else if (C2TR67CON.txenm && !C2TR67CON.txreqm && !C2TR67CON.rtrenm)
         port=6;
      else if (C2TR67CON.txenn && !C2TR67CON.txreqn && !C2TR67CON.rtrenn)
         port=7;
      else {
         #if CAN_DO_DEBUG
            can_debug("\r\nCAN2_PUTD() FAIL: NO OPEN TX BUFFERS\r\n");
         #endif
         return(0);
      }
      
      // load data into correct buffer
      if(ext)
      {
         temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan2_message_buffer[port][0]=temp;
         temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
         ecan2_message_buffer[port][1]=temp;
         temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
         ecan2_message_buffer[port][2]=temp;
      }
      else
      {  
         temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan2_message_buffer[port][0]=temp;
         ecan2_message_buffer[port][1]=0x0000;
         temp=(((uint16_t)rtr << 9) | len) & 0xFF1F;
         ecan2_message_buffer[port][2]=temp;
      }
      
      ptr=&ecan2_message_buffer[port][3];
      for(i=0;i<len;i+=2)
      {
         if (i+1==len)
            *ptr=make16(0,data[i]);
         else
            *ptr=make16(data[i+1],data[i]);
         ptr++;
      }
      
      switch(port)
      {
         case  0:
            C2TR01CON.txmpri=priority;          //set priority DMA buffer 0
            C2TR01CON.txreqm=1;                 //enable transmission buffer 0
            break;
         case 1:
            C2TR01CON.txnpri=priority;          //set priority DMA buffer 1
            C2TR01CON.txreqn=1;                 //enable transmission buffer 1
            break;
         case 2:
            C2TR23CON.txmpri=priority;          //set priority DMA buffer 2
            C2TR23CON.txreqm=1;                 //enable transmission buffer 2
            break;
         case 3:
            C2TR23CON.txnpri=priority;          //set priority DMA buffer 3
            C2TR23CON.txreqn=1;                 //enable transmission buffer 3
            break;
         case 4:
            C2TR45CON.txmpri=priority;          //set priority DMA buffer 4
            C2TR45CON.txreqm=1;                 //enable transmission buffer 4
            break;
         case 5:
            C2TR45CON.txnpri=priority;          //set priority DMA buffer 5
            C2TR45CON.txreqn=1;                 //enable transmission buffer 5
            break;
         case 6:
            C2TR67CON.txmpri=priority;          //set priority DMA buffer 6
            C2TR67CON.txreqm=1;                 //enable transmission buffer 6
            break;
         case 7:
            C2TR67CON.txnpri=priority;          //set priority DMA buffer 7
            C2TR67CON.txreqn=1;                 //enable transmission buffer 7
            break;
      }
          
      #if CAN_DO_DEBUG
               can_debug("\r\nCAN2_PUTD(): BUFF=%U ID=%LX LEN=%U PRI=%U EXT=%U RTR=%U\r\n", port, id, len, priority, ext, rtr);
               if ((len)&&(!rtr)) {
                  can_debug("  DATA = ");
                  for (i=0;i<len;i++) {
                     can_debug("%X ",*data);
                     data++;
                  }
                  can_debug("\r\n");
               }
      #endif 
   
      return(1);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_getd()
// can2_getd()
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
////////////////////////////////////////////////////////////////////////////////
int1 can_getd(uint32_t &id, uint8_t *data, uint8_t &len, struct rx_stat &stat)
{
   uint16_t i;
   uint16_t *ptr;
   int1 temp;
              
   if (C1RXFUL1.rxful0)
   {
      stat.buffer=0;
      stat.err_ovfl=C1RXOVF1.rxovf0;
      C1RXOVF1.rxovf0=0;
   }
   else if (C1RXFUL1.rxful1)
   {
      stat.buffer=1;
      stat.err_ovfl=C1RXOVF1.rxovf1;
      C1RXOVF1.rxovf1=0;
   }
   else if (C1RXFUL1.rxful2)
   {
      stat.buffer=2;
      stat.err_ovfl=C1RXOVF1.rxovf2;
      C1RXOVF1.rxovf2=0;
   }
   else if (C1RXFUL1.rxful3)
   {
      stat.buffer=3;
      stat.err_ovfl=C1RXOVF1.rxovf3;
      C1RXOVF1.rxovf3=0;
   }
   else if (C1RXFUL1.rxful4)
   {
      stat.buffer=4;
      stat.err_ovfl=C1RXOVF1.rxovf4;
      C1RXOVF1.rxovf4=0;
   }
   else if (C1RXFUL1.rxful5)
   {
      stat.buffer=5;
      stat.err_ovfl=C1RXOVF1.rxovf5;
      C1RXOVF1.rxovf5=0;
   }   
   else if (C1RXFUL1.rxful6)
   {
      stat.buffer=6;
      stat.err_ovfl=C1RXOVF1.rxovf6;
      C1RXOVF1.rxovf6=0;
   }
   else if (C1RXFUL1.rxful7)
   {
      stat.buffer=7;
      stat.err_ovfl=C1RXOVF1.rxovf7;
      C1RXOVF1.rxovf7=0;
   }
   else if (C1RXFUL1.rxful8)
   {
      stat.buffer=8;
      stat.err_ovfl=C1RXOVF1.rxovf8;
      C1RXOVF1.rxovf8=0;
   }
   else if (C1RXFUL1.rxful9)
   {
      stat.buffer=9;
      stat.err_ovfl=C1RXOVF1.rxovf9;
      C1RXOVF1.rxovf9=0;
   }
   else if (C1RXFUL1.rxful10)
   {
      stat.buffer=10;
      stat.err_ovfl=C1RXOVF1.rxovf10;
      C1RXOVF1.rxovf10=0;
   }
   else if (C1RXFUL1.rxful11)
   {
      stat.buffer=11;
      stat.err_ovfl=C1RXOVF1.rxovf11;
      C1RXOVF1.rxovf11=0;
   }
   else if (C1RXFUL1.rxful12)
   {
      stat.buffer=12;
      stat.err_ovfl=C1RXOVF1.rxovf12;
      C1RXOVF1.rxovf12=0;
   }
   else if (C1RXFUL1.rxful13)
   {
      stat.buffer=13;
      stat.err_ovfl=C1RXOVF1.rxovf13;
      C1RXOVF1.rxovf13=0;
   }
   else if (C1RXFUL1.rxful14)
   {
      stat.buffer=14;
      stat.err_ovfl=C1RXOVF1.rxovf14;
      C1RXOVF1.rxovf14=0;
   }
   else if (C1RXFUL1.rxful15)
   {
      stat.buffer=15;
      stat.err_ovfl=C1RXOVF1.rxovf15;
      C1RXOVF1.rxovf15=0;
   }
   else if (C1RXFUL2.rxful16)
   {
      stat.buffer=16;
      stat.err_ovfl=C1RXOVF2.rxovf16;
      C1RXOVF2.rxovf16=0;
   }
   else if (C1RXFUL2.rxful17)
   {
      stat.buffer=17;
      stat.err_ovfl=C1RXOVF2.rxovf17;
      C1RXOVF2.rxovf17=0;
   }
   else if (C1RXFUL2.rxful18)
   {
      stat.buffer=18;
      stat.err_ovfl=C1RXOVF2.rxovf18;
      C1RXOVF2.rxovf18=0;
   }
   else if (C1RXFUL2.rxful19)
   {
      stat.buffer=19;
      stat.err_ovfl=C1RXOVF2.rxovf19;
      C1RXOVF2.rxovf19=0;
   }
   else if (C1RXFUL2.rxful20)
   {
      stat.buffer=20;
      stat.err_ovfl=C1RXOVF2.rxovf20;
      C1RXOVF2.rxovf20=0;
   }
   else if (C1RXFUL2.rxful21)
   {
      stat.buffer=21;
      stat.err_ovfl=C1RXOVF2.rxovf21;
      C1RXOVF2.rxovf21=0;
   }
   else if (C1RXFUL2.rxful22)
   {
      stat.buffer=22;
      stat.err_ovfl=C1RXOVF2.rxovf22;
      C1RXOVF2.rxovf22=0;
   }
   else if (C1RXFUL2.rxful23)
   {
      stat.buffer=23;
      stat.err_ovfl=C1RXOVF2.rxovf23;
      C1RXOVF2.rxovf23=0;
   }
   else if (C1RXFUL2.rxful24)
   {
      stat.buffer=24;
      stat.err_ovfl=C1RXOVF2.rxovf24;
      C1RXOVF2.rxovf24=0;
   }
   else if (C1RXFUL2.rxful25)
   {
      stat.buffer=25;
      stat.err_ovfl=C1RXOVF2.rxovf25;
      C1RXOVF2.rxovf25=0;
   }
   else if (C1RXFUL2.rxful26)
   {
      stat.buffer=26;
      stat.err_ovfl=C1RXOVF2.rxovf26;
      C1RXOVF2.rxovf26=0;
   }
   else if (C1RXFUL2.rxful27)
   {
      stat.buffer=27;
      stat.err_ovfl=C1RXOVF2.rxovf27;
      C1RXOVF2.rxovf27=0;
   }
   else if (C1RXFUL2.rxful28)
   {
      stat.buffer=28;
      stat.err_ovfl=C1RXOVF2.rxovf28;
      C1RXOVF2.rxovf28=0;
   }
   else if (C1RXFUL2.rxful29)
   {
      stat.buffer=29;
      stat.err_ovfl=C1RXOVF2.rxovf29;
      C1RXOVF2.rxovf29=0;
   }
   else if (C1RXFUL2.rxful30)
   {
      stat.buffer=30;
      stat.err_ovfl=C1RXOVF2.rxovf30;
      C1RXOVF2.rxovf30=0;
   }
   else if (C1RXFUL2.rxful31)
   {
      stat.buffer=31;
      stat.err_ovfl=C1RXOVF2.rxovf31;
      C1RXOVF2.rxovf31=0;
   }
   else
    {
      #if CAN_DO_DEBUG
         can_debug("\r\nFAIL ON CAN_GETD(): NO MESSAGE IN BUFFER\r\n");
      #endif
      return (0);
    }
   
   C1INTF.rbif=0;
   
   ptr=&ecan1_message_buffer[stat.buffer][0];
   stat.ext=(*ptr & 0x1);
   temp=(*ptr & 0x1);
     
   ptr+=2;
   len=(*ptr & 0xF);
   stat.rtr=((make8(*ptr,1) & 0x2) >> 1);
   
   ptr+=5;
   stat.filthit=(make8(*ptr,1) & 0x1F);
      
   id=can_get_id(stat.buffer,temp);
   
   ptr=&ecan1_message_buffer[stat.buffer][3];
   for(i=0;i<len;i+=2)
   {
      data[i]=make8(*ptr,0);
      if (i+1!=len)
         data[i+1]=make8(*ptr,1);
      ptr++;
   }
   
   stat.inv=C1INTF.ivrif;
   C1INTF.ivrif=0;
   
   switch(stat.buffer)
   {
      case 0:
         C1RXFUL1.rxful0=0;
         break;
      case 1:
         C1RXFUL1.rxful1=0;
         break;
      case 2:
         C1RXFUL1.rxful2=0;
         break;
      case 3:
         C1RXFUL1.rxful3=0;
         break;
      case 4:
         C1RXFUL1.rxful4=0;
         break;
      case 5:
         C1RXFUL1.rxful5=0;
         break;
      case 6:
         C1RXFUL1.rxful6=0;
         break;
      case 7:
         C1RXFUL1.rxful7=0;
         break;
      case 8:
         C1RXFUL1.rxful8=0;
         break;
      case 9:
         C1RXFUL1.rxful9=0;
         break;
      case 10:
         C1RXFUL1.rxful10=0;
         break;
      case 11:
         C1RXFUL1.rxful11=0;
         break;
      case 12:
         C1RXFUL1.rxful12=0;
         break;
      case 13:
         C1RXFUL1.rxful13=0;
         break;
      case 14:
         C1RXFUL1.rxful14=0;
         break;
      case 15:
         C1RXFUL1.rxful15=0;
         break;
      case 16:
         C1RXFUL2.rxful16=0;
         break;
      case 17:
         C1RXFUL2.rxful17=0;
         break;
      case 18:
         C1RXFUL2.rxful18=0;
         break;
      case 19:
         C1RXFUL2.rxful19=0;
         break;
      case 20:
         C1RXFUL2.rxful20=0;
         break;
      case 21:
         C1RXFUL2.rxful21=0;
         break;
      case 22:
         C1RXFUL2.rxful22=0;
         break;
      case 23:
         C1RXFUL2.rxful23=0;
         break;
      case 24:
         C1RXFUL2.rxful24=0;
         break;
      case 25:
         C1RXFUL2.rxful25=0;
         break;
      case 26:
         C1RXFUL2.rxful26=0;
         break;
      case 27:
         C1RXFUL2.rxful27=0;
         break;
      case 28:
         C1RXFUL2.rxful28=0;
         break;
      case 29:
         C1RXFUL2.rxful29=0;
         break;
      case 30:
         C1RXFUL2.rxful30=0;
         break;
      case 31:
         C1RXFUL2.rxful31=0;
         break;
   }      
   
   #if CAN_DO_DEBUG
       can_debug("\r\nCAN_GETD(): BUFF=%U ID=%LX LEN=%U OVF=%U ", stat.buffer, id, len, stat.err_ovfl);
       can_debug("FILT=%U RTR=%U EXT=%U INV=%U", stat.filthit, stat.rtr, stat.ext, stat.inv);
       if ((len)&&(!stat.rtr)) {
          can_debug("\r\n    DATA = ");
          for (i=0;i<len;i++) {
            can_debug("%X ",*data);
            data++;
          }
       }
       can_debug("\r\n");
    #endif 
    
    return(1);
}

#if getenv("SFR:C2CTRL1")
   int1 can2_getd(uint32_t &id, uint8_t *data, uint8_t &len, struct rx_stat &stat)
   {
      uint16_t i;
      uint16_t *ptr;
      int1 temp;
                 
      if (C2RXFUL1.rxful0)
      {
         stat.buffer=0;
         stat.err_ovfl=C2RXOVF1.rxovf0;
         C2RXOVF1.rxovf0=0;
      }
      else if (C2RXFUL1.rxful1)
      {
         stat.buffer=1;
         stat.err_ovfl=C2RXOVF1.rxovf1;
         C2RXOVF1.rxovf1=0;
      }
      else if (C2RXFUL1.rxful2)
      {
         stat.buffer=2;
         stat.err_ovfl=C2RXOVF1.rxovf2;
         C2RXOVF1.rxovf2=0;
      }
      else if (C2RXFUL1.rxful3)
      {
         stat.buffer=3;
         stat.err_ovfl=C2RXOVF1.rxovf3;
         C2RXOVF1.rxovf3=0;
      }
      else if (C2RXFUL1.rxful4)
      {
         stat.buffer=4;
         stat.err_ovfl=C2RXOVF1.rxovf4;
         C2RXOVF1.rxovf4=0;
      }
      else if (C2RXFUL1.rxful5)
      {
         stat.buffer=5;
         stat.err_ovfl=C2RXOVF1.rxovf5;
         C2RXOVF1.rxovf5=0;
      }   
      else if (C2RXFUL1.rxful6)
      {
         stat.buffer=6;
         stat.err_ovfl=C2RXOVF1.rxovf6;
         C2RXOVF1.rxovf6=0;
      }
      else if (C2RXFUL1.rxful7)
      {
         stat.buffer=7;
         stat.err_ovfl=C2RXOVF1.rxovf7;
         C2RXOVF1.rxovf7=0;
      }
      else if (C2RXFUL1.rxful8)
      {
         stat.buffer=8;
         stat.err_ovfl=C2RXOVF1.rxovf8;
         C2RXOVF1.rxovf8=0;
      }
      else if (C2RXFUL1.rxful9)
      {
         stat.buffer=9;
         stat.err_ovfl=C2RXOVF1.rxovf9;
         C2RXOVF1.rxovf9=0;
      }
      else if (C2RXFUL1.rxful10)
      {
         stat.buffer=10;
         stat.err_ovfl=C2RXOVF1.rxovf10;
         C2RXOVF1.rxovf10=0;
      }
      else if (C2RXFUL1.rxful11)
      {
         stat.buffer=11;
         stat.err_ovfl=C2RXOVF1.rxovf11;
         C2RXOVF1.rxovf11=0;
      }
      else if (C2RXFUL1.rxful12)
      {
         stat.buffer=12;
         stat.err_ovfl=C2RXOVF1.rxovf12;
         C2RXOVF1.rxovf12=0;
      }
      else if (C2RXFUL1.rxful13)
      {
         stat.buffer=13;
         stat.err_ovfl=C2RXOVF1.rxovf13;
         C2RXOVF1.rxovf13=0;
      }
      else if (C2RXFUL1.rxful14)
      {
         stat.buffer=14;
         stat.err_ovfl=C2RXOVF1.rxovf14;
         C2RXOVF1.rxovf14=0;
      }
      else if (C2RXFUL1.rxful15)
      {
         stat.buffer=15;
         stat.err_ovfl=C2RXOVF1.rxovf15;
         C2RXOVF1.rxovf15=0;
      }
      else if (C2RXFUL2.rxful16)
      {
         stat.buffer=16;
         stat.err_ovfl=C2RXOVF2.rxovf16;
         C2RXOVF2.rxovf16=0;
      }
      else if (C2RXFUL2.rxful17)
      {
         stat.buffer=17;
         stat.err_ovfl=C2RXOVF2.rxovf17;
         C2RXOVF2.rxovf17=0;
      }
      else if (C2RXFUL2.rxful18)
      {
         stat.buffer=18;
         stat.err_ovfl=C2RXOVF2.rxovf18;
         C2RXOVF2.rxovf18=0;
      }
      else if (C2RXFUL2.rxful19)
      {
         stat.buffer=19;
         stat.err_ovfl=C2RXOVF2.rxovf19;
         C2RXOVF2.rxovf19=0;
      }
      else if (C2RXFUL2.rxful20)
      {
         stat.buffer=20;
         stat.err_ovfl=C2RXOVF2.rxovf20;
         C2RXOVF2.rxovf20=0;
      }
      else if (C2RXFUL2.rxful21)
      {
         stat.buffer=21;
         stat.err_ovfl=C2RXOVF2.rxovf21;
         C2RXOVF2.rxovf21=0;
      }
      else if (C2RXFUL2.rxful22)
      {
         stat.buffer=22;
         stat.err_ovfl=C2RXOVF2.rxovf22;
         C2RXOVF2.rxovf22=0;
      }
      else if (C2RXFUL2.rxful23)
      {
         stat.buffer=23;
         stat.err_ovfl=C2RXOVF2.rxovf23;
         C2RXOVF2.rxovf23=0;
      }
      else if (C2RXFUL2.rxful24)
      {
         stat.buffer=24;
         stat.err_ovfl=C2RXOVF2.rxovf24;
         C2RXOVF2.rxovf24=0;
      }
      else if (C2RXFUL2.rxful25)
      {
         stat.buffer=25;
         stat.err_ovfl=C2RXOVF2.rxovf25;
         C2RXOVF2.rxovf25=0;
      }
      else if (C2RXFUL2.rxful26)
      {
         stat.buffer=26;
         stat.err_ovfl=C2RXOVF2.rxovf26;
         C2RXOVF2.rxovf26=0;
      }
      else if (C2RXFUL2.rxful27)
      {
         stat.buffer=27;
         stat.err_ovfl=C2RXOVF2.rxovf27;
         C2RXOVF2.rxovf27=0;
      }
      else if (C2RXFUL2.rxful28)
      {
         stat.buffer=28;
         stat.err_ovfl=C2RXOVF2.rxovf28;
         C2RXOVF2.rxovf28=0;
      }
      else if (C2RXFUL2.rxful29)
      {
         stat.buffer=29;
         stat.err_ovfl=C2RXOVF2.rxovf29;
         C2RXOVF2.rxovf29=0;
      }
      else if (C2RXFUL2.rxful30)
      {
         stat.buffer=30;
         stat.err_ovfl=C2RXOVF2.rxovf30;
         C2RXOVF2.rxovf30=0;
      }
      else if (C2RXFUL2.rxful31)
      {
         stat.buffer=31;
         stat.err_ovfl=C2RXOVF2.rxovf31;
         C2RXOVF2.rxovf31=0;
      }
      else
       {
         #if CAN_DO_DEBUG
            can_debug("\r\nFAIL ON CAN2_GETD(): NO MESSAGE IN BUFFER\r\n");
         #endif
         return (0);
       }
      
      C2INTF.rbif=0;
      
      ptr=&ecan2_message_buffer[stat.buffer][0];
      stat.ext=(*ptr & 0x1);
      temp=(*ptr & 0x1);
        
      ptr+=2;
      len=(*ptr & 0xF);
      stat.rtr=((make8(*ptr,1) & 0x2) >> 1);
      
      ptr+=5;
      stat.filthit=(make8(*ptr,1) & 0x1F);
         
      id=can2_get_id(stat.buffer,temp);
      
      ptr=&ecan2_message_buffer[stat.buffer][3];
      for(i=0;i<len;i+=2)
      {
         data[i]=make8(*ptr,0);
         if (i+1!=len)
            data[i+1]=make8(*ptr,1);
         ptr++;
      }
      
      stat.inv=C2INTF.ivrif;
      C2INTF.ivrif=0;
      
      switch(stat.buffer)
      {
         case 0:
            C2RXFUL1.rxful0=0;
            break;
         case 1:
            C2RXFUL1.rxful1=0;
            break;
         case 2:
            C2RXFUL1.rxful2=0;
            break;
         case 3:
            C2RXFUL1.rxful3=0;
            break;
         case 4:
            C2RXFUL1.rxful4=0;
            break;
         case 5:
            C2RXFUL1.rxful5=0;
            break;
         case 6:
            C2RXFUL1.rxful6=0;
            break;
         case 7:
            C2RXFUL1.rxful7=0;
            break;
         case 8:
            C2RXFUL1.rxful8=0;
            break;
         case 9:
            C2RXFUL1.rxful9=0;
            break;
         case 10:
            C2RXFUL1.rxful10=0;
            break;
         case 11:
            C2RXFUL1.rxful11=0;
            break;
         case 12:
            C2RXFUL1.rxful12=0;
            break;
         case 13:
            C2RXFUL1.rxful13=0;
            break;
         case 14:
            C2RXFUL1.rxful14=0;
            break;
         case 15:
            C2RXFUL1.rxful15=0;
            break;
         case 16:
            C2RXFUL2.rxful16=0;
            break;
         case 17:
            C2RXFUL2.rxful17=0;
            break;
         case 18:
            C2RXFUL2.rxful18=0;
            break;
         case 19:
            C2RXFUL2.rxful19=0;
            break;
         case 20:
            C2RXFUL2.rxful20=0;
            break;
         case 21:
            C2RXFUL2.rxful21=0;
            break;
         case 22:
            C2RXFUL2.rxful22=0;
            break;
         case 23:
            C2RXFUL2.rxful23=0;
            break;
         case 24:
            C2RXFUL2.rxful24=0;
            break;
         case 25:
            C2RXFUL2.rxful25=0;
            break;
         case 26:
            C2RXFUL2.rxful26=0;
            break;
         case 27:
            C2RXFUL2.rxful27=0;
            break;
         case 28:
            C2RXFUL2.rxful28=0;
            break;
         case 29:
            C2RXFUL2.rxful29=0;
            break;
         case 30:
            C2RXFUL2.rxful30=0;
            break;
         case 31:
            C2RXFUL2.rxful31=0;
            break;
      }      
      
      #if CAN_DO_DEBUG
          can_debug("\r\nCAN2_GETD(): BUFF=%U ID=%LX LEN=%U OVF=%U ", stat.buffer, id, len, stat.err_ovfl);
          can_debug("FILT=%U RTR=%U EXT=%U INV=%U", stat.filthit, stat.rtr, stat.ext, stat.inv);
          if ((len)&&(!stat.rtr)) {
             can_debug("\r\n    DATA = ");
             for (i=0;i<len;i++) {
               can_debug("%X ",*data);
               data++;
             }
          }
          can_debug("\r\n");
       #endif 
       
       return(1);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_get_id()
// can2_get_id()
//
// Returns the ID of the specified buffer. (The opposite of can_set_buffer_id())
// This is used after receiving a message, to see which ID sent the message.
//
//   Paramaters:
//     buffer - number of specified buffer
//      enumerated as TRB0-TRB7 and RB8-RB31
//     ext - Set to TRUE if this buffer uses an extended ID, FALSE if not
//
//   Returns:
//     The ID of the buffer
//
////////////////////////////////////////////////////////////////////////////////
uint32_t can_get_id(BUFFER buffer, int1 ext)
{
   uint32_t ret=0;
   uint16_t *ptr;
   uint16_t temp, temp1;
   
   ptr=&ecan1_message_buffer[buffer][2];
      
   if (ext)
   {
      //EID
      temp=((*ptr & 0xFC00) >> 10)& 0x3F;
      
      ptr-=1;
      temp|=((*ptr & 0x3FF) << 6);
      temp1=(*ptr&0xC00)>>10;
      
      //SID
      ptr-=1;
      temp1|=*ptr&0x1FFC;
      ret=make32(temp1,temp);
   }
   else 
   {
      //SID
      ptr-=2;
      ret=((*ptr & 0x1FFC) >> 2) & 0x7FF;
   }
   
   return(ret);
}

#if getenv("SFR:C2CTRL1")
   uint32_t can2_get_id(BUFFER buffer, int1 ext)
   {
      uint32_t ret=0;
      uint16_t *ptr;
      uint16_t temp, temp1;
      
      ptr=&ecan2_message_buffer[buffer][2];
         
      if (ext)
      {
         //EID
         temp=((*ptr & 0xFC00) >> 10)& 0x3F;
         
         ptr-=1;
         temp|=((*ptr & 0x3FF) << 6);
         temp1=(*ptr&0xC00)>>10;
         
         //SID
         ptr-=1;
         temp1|=*ptr&0x1FFC;
         ret=make32(temp1,temp);
      }
      else 
      {
         //SID
         ptr-=2;
         ret=((*ptr & 0x1FFC) >> 2) & 0x7FF;
      }
      
      return(ret);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_enable_b_transfer()
// can2_enable_b_transfer()
//
// Sets one of the 8 programmable buffers to be a transfer buffer, and sets the 
// start of the FIFO buffer to be the first buffer after the highest buffer set 
// as a transmit buffer. 
//
// Parameters:
//      b - the buffer which is to become a transfer buffer
//       enumerated as TRB0-TRB7
//
////////////////////////////////////////////////////////////////////////////////
void can_enable_b_transfer(BUFFER b)
{
   switch(b)
   {
      case 0:
         C1TR01CON.txenm=1;
         break;
      case 1:
         C1TR01CON.txenn=1;
         break;
      case 2:
         C1TR23CON.txenm=1;
         break;
      case 3:
         C1TR23CON.txenn=1;
         break;
      case 4:
         C1TR45CON.txenm=1;
         break;
      case 5:
         C1TR45CON.txenn=1;
         break;
      case 6:
         C1TR67CON.txenm=1;
         break;
      case 7:
         C1TR67CON.txenn=1;
         break;
   }
   if(C1TR67CON.txenn)
      C1FCTRL.fsa=8;
   else if (C1TR67CON.txenm)
      C1FCTRL.fsa=7;
   else if (C1TR45CON.txenn)
      C1FCTRL.fsa=6;
   else if (C1TR45CON.txenm)
      C1FCTRL.fsa=5;
   else if (C1TR23CON.txenn)
      C1FCTRL.fsa=4;
   else if (C1TR23CON.txenm)
      C1FCTRL.fsa=3;
   else if (C1TR01CON.txenn)
      C1FCTRL.fsa=2;
   else 
      C1FCTRL.fsa=1;   
}

#if getenv("SFR:C2CTRL1")
   void can2_enable_b_transfer(BUFFER b)
   {
      switch(b)
      {
         case 0:
            C2TR01CON.txenm=1;
            break;
         case 1:
            C2TR01CON.txenn=1;
            break;
         case 2:
            C2TR23CON.txenm=1;
            break;
         case 3:
            C2TR23CON.txenn=1;
            break;
         case 4:
            C2TR45CON.txenm=1;
            break;
         case 5:
            C2TR45CON.txenn=1;
            break;
         case 6:
            C2TR67CON.txenm=1;
            break;
         case 7:
            C2TR67CON.txenn=1;
            break;
      }
      if(C2TR67CON.txenn)
         C2FCTRL.fsa=8;
      else if (C2TR67CON.txenm)
         C2FCTRL.fsa=7;
      else if (C2TR45CON.txenn)
         C2FCTRL.fsa=6;
      else if (C2TR45CON.txenm)
         C2FCTRL.fsa=5;
      else if (C2TR23CON.txenn)
         C2FCTRL.fsa=4;
      else if (C2TR23CON.txenm)
         C2FCTRL.fsa=3;
      else if (C2TR01CON.txenn)
         C2FCTRL.fsa=2;
      else 
         C2FCTRL.fsa=1;   
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_enable_b_receiver()
// can2_enable_b_receiver()
//
// Sets one of the 8 programmable buffers to be a receiver buffer, and sets the 
// start of the FIFO buffer to be the first buffer after the highest buffer set 
// as a transmit buffer.
//
// Parameters:
//       b - the buffer which is to become a receiver buffer
//        enumerated as TRB0-TRB7
//
////////////////////////////////////////////////////////////////////////////////
void can_enable_b_receiver(BUFFER b)
{
   switch(b)
   {
      case 0:
         C1TR01CON.txenm=0;
         break;
      case 1:
         C1TR01CON.txenn=0;
         break;
      case 2:
         C1TR23CON.txenm=0;
         break;
      case 3:
         C1TR23CON.txenn=0;
         break;
      case 4:
         C1TR45CON.txenm=0;
         break;
      case 5:
         C1TR45CON.txenn=0;
         break;
      case 6:
         C1TR67CON.txenm=0;
         break;
      case 7:
         C1TR67CON.txenn=0;
         break;
   }
   if(C1TR67CON.txenn)
      C1FCTRL.fsa=8;
   else if (C1TR67CON.txenm)
      C1FCTRL.fsa=7;
   else if (C1TR45CON.txenn)
      C1FCTRL.fsa=6;
   else if (C1TR45CON.txenm)
      C1FCTRL.fsa=5;
   else if (C1TR23CON.txenn)
      C1FCTRL.fsa=4;
   else if (C1TR23CON.txenm)
      C1FCTRL.fsa=3;
   else if (C1TR01CON.txenn)
      C1FCTRL.fsa=2;
   else if (C1TR01CON.txenm)
      C1FCTRL.fsa=1;
   else
      C1FCTRL.fsa=0;
}

#if getenv("SFR:C2CTRL1")
   void can2_enable_b_receiver(BUFFER b)
   {
      switch(b)
      {
         case 0:
            C2TR01CON.txenm=0;
            break;
         case 1:
            C2TR01CON.txenn=0;
            break;
         case 2:
            C2TR23CON.txenm=0;
            break;
         case 3:
            C2TR23CON.txenn=0;
            break;
         case 4:
            C2TR45CON.txenm=0;
            break;
         case 5:
            C2TR45CON.txenn=0;
            break;
         case 6:
            C2TR67CON.txenm=0;
            break;
         case 7:
            C2TR67CON.txenn=0;
            break;
      }
      if(C2TR67CON.txenn)
         C2FCTRL.fsa=8;
      else if (C2TR67CON.txenm)
         C2FCTRL.fsa=7;
      else if (C2TR45CON.txenn)
         C2FCTRL.fsa=6;
      else if (C2TR45CON.txenm)
         C2FCTRL.fsa=5;
      else if (C2TR23CON.txenn)
         C2FCTRL.fsa=4;
      else if (C2TR23CON.txenm)
         C2FCTRL.fsa=3;
      else if (C2TR01CON.txenn)
         C2FCTRL.fsa=2;
      else if (C2TR01CON.txenm)
         C2FCTRL.fsa=1;
      else
         C2FCTRL.fsa=0;
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_enable_rtr()
// can2_enable_rtr()
//
// Sets the desired programmable buffer to be used as an Auto-rtr buffer.
//
// Parameters
//       b - the programmable buffer to be set to Auto-rtr mode
//       enumerated as TRB0-TRB7
//
////////////////////////////////////////////////////////////////////////////////
void can_enable_rtr(BUFFER b)
{
   switch(b)
   {
      case 0:
         C1TR01CON.rtrenm=1;
         break;
      case 1:
         C1TR01CON.rtrenn=1;
         break;
      case 2:
         C1TR23CON.rtrenm=1;
         break;
      case 3:
         C1TR23CON.rtrenn=1;
         break;
      case 4:
         C1TR45CON.rtrenm=1;
         break;
      case 5:
         C1TR45CON.rtrenn=1;
         break;
      case 6:
         C1TR67CON.rtrenm=1;
         break;
      case 7:
         C1TR67CON.rtrenn=1;
         break;
   }
}

#if getenv("SFR:C2CTRL1")
   void can2_enable_rtr(BUFFER b)
   {
      switch(b)
      {
         case 0:
            C2TR01CON.rtrenm=1;
            break;
         case 1:
            C2TR01CON.rtrenn=1;
            break;
         case 2:
            C2TR23CON.rtrenm=1;
            break;
         case 3:
            C2TR23CON.rtrenn=1;
            break;
         case 4:
            C2TR45CON.rtrenm=1;
            break;
         case 5:
            C2TR45CON.rtrenn=1;
            break;
         case 6:
            C2TR67CON.rtrenm=1;
            break;
         case 7:
            C2TR67CON.rtrenn=1;
            break;
      }
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_disable_rtr()
// can2_disable_rtr()
//
// Disables specified programmable buffer from being used as Auto-rtr buffer.
//
// Parameters:
//      b - the programmable buffer to be disabled from Auto-rtr mode
//       enumerated as TRB0-TRB7
//
////////////////////////////////////////////////////////////////////////////////
void can_disable_rtr(BUFFER b)
{
   switch(b)
   {
      case 0:
         C1TR01CON.rtrenm=0;
         break;
      case 1:
         C1TR01CON.rtrenn=0;
         break;
      case 2:
         C1TR23CON.rtrenm=0;
         break;
      case 3:
         C1TR23CON.rtrenn=0;
         break;
      case 4:
         C1TR45CON.rtrenm=0;
         break;
      case 5:
         C1TR45CON.rtrenn=0;
         break;
      case 6:
         C1TR67CON.rtrenm=0;
         break;
      case 7:
         C1TR67CON.rtrenn=0;
         break;
   }
}

#if getenv("SFR:C2CTRL1")
   void can2_disable_rtr(BUFFER b)
   {
      switch(b)
      {
         case 0:
            C2TR01CON.rtrenm=0;
            break;
         case 1:
            C2TR01CON.rtrenn=0;
            break;
         case 2:
            C2TR23CON.rtrenm=0;
            break;
         case 3:
            C2TR23CON.rtrenn=0;
            break;
         case 4:
            C2TR45CON.rtrenm=0;
            break;
         case 5:
            C2TR45CON.rtrenn=0;
            break;
         case 6:
            C2TR67CON.rtrenm=0;
            break;
         case 7:
            C2TR67CON.rtrenn=0;
            break;
      }
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_load_rtr()
// can2_load_rtr()
//
//   Loads a SINGLE b buffer with data to be sent on the occurence of an rtr.
//
// Parameters:
//
//      b - The programmable buffer to be filled
//       enumerated as TRB0-TRB7
//      data - the address of the first data byte
//      len - the number of data bytes to be copied
//
// Returns:
//      void
//
//   Note, the programmer must set the appropriate rtr registers in order for the
//         rtr function to work.  This function does not set any of those
//         registers.  For example, to enable TRB0 as an Auto-rtr buffer, the
//         functions:
//
//         can_enable_b_transfer(TRB0);
//         can_enable_rtr(TRB0);
//         function to link acceptance filter to TRB0
//
//         needs to be called.
//
////////////////////////////////////////////////////////////////////////////////
void can_load_rtr (BUFFER b, uint8_t *data, uint8_t len)
{
   uint16_t i;
   uint16_t *ptr;
   
   curmode=C1CTRL1.opmode;
   
   can_set_mode(CAN_OP_CONFIG);
   
   ptr=&ecan1_message_buffer[b][2];
   
   *ptr&=0xFFF0;
   *ptr|=len;
   
   for(i=0;i<len;i+=2)
   {
      ptr+=1;
      if (i+1==len)
         *ptr=make16(0,data[i]);
      else
         *ptr=make16(data[i+1],data[i]);
   }
   
   can_set_mode(curmode);
}

#if getenv("SFR:C2CTRL1")
   void can2_load_rtr (BUFFER b, uint8_t *data, uint8_t len)
   {
      uint16_t i;
      uint16_t *ptr;
      
      curmode=C2CTRL1.opmode;
      
      can2_set_mode(CAN_OP_CONFIG);
      
      ptr=&ecan2_message_buffer[b][2];
      
      *ptr&=0xFFF0;
      *ptr|=len;
      
      for(i=0;i<len;i+=2)
      {
         ptr+=1;
         if (i+1==len)
            *ptr=make16(0,data[i]);
         else
            *ptr=make16(data[i+1],data[i]);
      }
      
      can2_set_mode(curmode);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_fifo_getd()
// can2_fifo_getd()
//
//    Retrieves data in FIFO mode
//
// Parameters:
//      id - The ID of the sender
//      data - Address of the array to store the data in
//      len - number of data bytes to read
//      stat - status structure to return infromation about the receive register
//
// Returns:
//      int1 - TRUE if there was data in the buffer, FALSE if there wasn't
//
////////////////////////////////////////////////////////////////////////////////
int1 can_fifo_getd(uint32_t &id, uint8_t *data, uint8_t &len, struct rx_stat &stat )
{
   uint16_t i, temp1;
   uint16_t *ptr;
   int1 temp;
   
   temp1=C1FIFO.fnrb;
   
   switch(temp1)
   {
      case 0:
         if(!C1RXFUL1.rxful0)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf0;
            C1RXOVF1.rxovf0=0;
         }
         break;
      case 1:
         if(!C1RXFUL1.rxful1)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf1;
            C1RXOVF1.rxovf1=0;
         }
         break;
      case 2:
         if(!C1RXFUL1.rxful2)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf2;
            C1RXOVF1.rxovf2=0;
         }
         break;
      case 3:
         if(!C1RXFUL1.rxful3)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf3;
            C1RXOVF1.rxovf3=0;
         }
         break;
      case 4:
         if(!C1RXFUL1.rxful4)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf4;
            C1RXOVF1.rxovf4=0;
         }
         break;
      case 5:
         if(!C1RXFUL1.rxful5)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf5;
            C1RXOVF1.rxovf5=0;
         }
         break;
      case 6:
         if(!C1RXFUL1.rxful6)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf6;
            C1RXOVF1.rxovf6=0;
         }
         break;
      case 7:
         if(!C1RXFUL1.rxful7)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf7;
            C1RXOVF1.rxovf7=0;
         }
         break;
      case 8:
         if(!C1RXFUL1.rxful8)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf8;
            C1RXOVF1.rxovf8=0;
         }
         break;
      case 9:
         if(!C1RXFUL1.rxful9)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf9;
            C1RXOVF1.rxovf9=0;
         }
         break;
      case 10:
         if(!C1RXFUL1.rxful10)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf10;
            C1RXOVF1.rxovf10=0;
         }
         break;
      case 11:
         if(!C1RXFUL1.rxful11)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf11;
            C1RXOVF1.rxovf11=0;
         }
         break;
      case 12:
         if(!C1RXFUL1.rxful12)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf12;
            C1RXOVF1.rxovf12=0;
         }
         break;
      case 13:
         if(!C1RXFUL1.rxful13)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf13;
            C1RXOVF1.rxovf13=0;
         }
         break;
      case 14:
         if(!C1RXFUL1.rxful14)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf14;
            C1RXOVF1.rxovf14=0;
         }
         break;
      case 15:
         if(!C1RXFUL1.rxful15)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF1.rxovf15;
            C1RXOVF1.rxovf15=0;
         }
         break;
      case 16:
         if(!C1RXFUL2.rxful16)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf16;
            C1RXOVF2.rxovf16=0;
         }
         break;
      case 17:
         if(!C1RXFUL2.rxful17)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf17;
            C1RXOVF2.rxovf17=0;
         }
         break;
      case 18:
         if(!C1RXFUL2.rxful18)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf18;
            C1RXOVF2.rxovf18=0;
         }
         break;
      case 19:
         if(!C1RXFUL2.rxful19)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf19;
            C1RXOVF2.rxovf19=0;
         }
         break;
      case 20:
         if(!C1RXFUL2.rxful20)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf20;
            C1RXOVF2.rxovf20=0;
         }
         break;
      case 21:
         if(!C1RXFUL2.rxful21)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf21;
            C1RXOVF2.rxovf21=0;
         }
         break;
      case 22:
         if(!C1RXFUL2.rxful22)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf22;
            C1RXOVF2.rxovf22=0;
         }
         break;
      case 23:
         if(!C1RXFUL2.rxful23)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf23;
            C1RXOVF2.rxovf23=0;
         }
         break;
      case 24:
         if(!C1RXFUL2.rxful24)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf24;
            C1RXOVF2.rxovf24=0;
         }
         break;
      case 25:
         if(!C1RXFUL2.rxful25)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf25;
            C1RXOVF2.rxovf25=0;
         }
         break;
      case 26:
         if(!C1RXFUL2.rxful26)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf26;
            C1RXOVF2.rxovf26=0;
         }
         break;
      case 27:
         if(!C1RXFUL2.rxful27)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf27;
            C1RXOVF2.rxovf27=0;
         }
         break;
      case 28:
         if(!C1RXFUL2.rxful28)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf28;
            C1RXOVF2.rxovf28=0;
         }
         break;
      case 29:
         if(!C1RXFUL2.rxful29)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf29;
            C1RXOVF2.rxovf29=0;
         }
         break;
      case 30:
         if(!C1RXFUL2.rxful30)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf30;
            C1RXOVF2.rxovf30=0;
         }
         break;
      case 31:
         if(!C1RXFUL2.rxful31)
            return(0);
         else {
            stat.err_ovfl=C1RXOVF2.rxovf31;
            C1RXOVF2.rxovf31=0;
         }
         break;
   }
   
   C1INTF.rbif=0;
   
   stat.buffer=C1FIFO.fnrb;
   
   ptr=&ecan1_message_buffer[stat.buffer][0];
   stat.ext=(*ptr & 0x1);
   temp=(*ptr & 0x1);
     
   ptr+=2;
   len=(*ptr & 0xF);
   stat.rtr=((make8(*ptr,1) & 0x2) >> 1);
   
   ptr+=5;
   stat.filthit=(make8(*ptr,1) & 0x1F);
      
   id=can_get_id(stat.buffer,temp);
   
   ptr=&ecan1_message_buffer[stat.buffer][3];
   for(i=0;i<len;i+=2)
   {
      data[i]=make8(*ptr,0);
      if (i+1!=len)
         data[i+1]=make8(*ptr,1);
      ptr++;
   }
   
   stat.inv=C1INTF.ivrif;
   C1INTF.ivrif=0;
   
   switch(stat.buffer)
   {
      case 0:
         C1RXFUL1.rxful0=0;
         break;
      case 1:
         C1RXFUL1.rxful1=0;
         break;
      case 2:
         C1RXFUL1.rxful2=0;
         break;
      case 3:
         C1RXFUL1.rxful3=0;
         break;
      case 4:
         C1RXFUL1.rxful4=0;
         break;
      case 5:
         C1RXFUL1.rxful5=0;
         break;
      case 6:
         C1RXFUL1.rxful6=0;
         break;
      case 7:
         C1RXFUL1.rxful7=0;
         break;
      case 8:
         C1RXFUL1.rxful8=0;
         break;
      case 9:
         C1RXFUL1.rxful9=0;
         break;
      case 10:
         C1RXFUL1.rxful10=0;
         break;
      case 11:
         C1RXFUL1.rxful11=0;
         break;
      case 12:
         C1RXFUL1.rxful12=0;
         break;
      case 13:
         C1RXFUL1.rxful13=0;
         break;
      case 14:
         C1RXFUL1.rxful14=0;
         break;
      case 15:
         C1RXFUL1.rxful15=0;
         break;
      case 16:
         C1RXFUL2.rxful16=0;
         break;
      case 17:
         C1RXFUL2.rxful17=0;
         break;
      case 18:
         C1RXFUL2.rxful18=0;
         break;
      case 19:
         C1RXFUL2.rxful19=0;
         break;
      case 20:
         C1RXFUL2.rxful20=0;
         break;
      case 21:
         C1RXFUL2.rxful21=0;
         break;
      case 22:
         C1RXFUL2.rxful22=0;
         break;
      case 23:
         C1RXFUL2.rxful23=0;
         break;
      case 24:
         C1RXFUL2.rxful24=0;
         break;
      case 25:
         C1RXFUL2.rxful25=0;
         break;
      case 26:
         C1RXFUL2.rxful26=0;
         break;
      case 27:
         C1RXFUL2.rxful27=0;
         break;
      case 28:
         C1RXFUL2.rxful28=0;
         break;
      case 29:
         C1RXFUL2.rxful29=0;
         break;
      case 30:
         C1RXFUL2.rxful30=0;
         break;
      case 31:
         C1RXFUL2.rxful31=0;
         break;
   }
   
   return(1);
}

#if getenv("SFR:C2CTRL1")
   int1 can2_fifo_getd(uint32_t &id, uint8_t *data, uint8_t &len, struct rx_stat &stat )
   {
      uint16_t i, temp1;
      uint16_t *ptr;
      int1 temp;
      
      temp1=C2FIFO.fnrb;
      
      switch(temp1)
      {
         case 0:
            if(!C2RXFUL1.rxful0)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf0;
               C2RXOVF1.rxovf0=0;
            }
            break;
         case 1:
            if(!C2RXFUL1.rxful1)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf1;
               C2RXOVF1.rxovf1=0;
            }
            break;
         case 2:
            if(!C2RXFUL1.rxful2)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf2;
               C2RXOVF1.rxovf2=0;
            }
            break;
         case 3:
            if(!C2RXFUL1.rxful3)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf3;
               C2RXOVF1.rxovf3=0;
            }
            break;
         case 4:
            if(!C2RXFUL1.rxful4)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf4;
               C2RXOVF1.rxovf4=0;
            }
            break;
         case 5:
            if(!C2RXFUL1.rxful5)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf5;
               C2RXOVF1.rxovf5=0;
            }
            break;
         case 6:
            if(!C2RXFUL1.rxful6)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf6;
               C2RXOVF1.rxovf6=0;
            }
            break;
         case 7:
            if(!C2RXFUL1.rxful7)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf7;
               C2RXOVF1.rxovf7=0;
            }
            break;
         case 8:
            if(!C2RXFUL1.rxful8)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf8;
               C2RXOVF1.rxovf8=0;
            }
            break;
         case 9:
            if(!C2RXFUL1.rxful9)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf9;
               C2RXOVF1.rxovf9=0;
            }
            break;
         case 10:
            if(!C2RXFUL1.rxful10)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf10;
               C2RXOVF1.rxovf10=0;
            }
            break;
         case 11:
            if(!C2RXFUL1.rxful11)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf11;
               C2RXOVF1.rxovf11=0;
            }
            break;
         case 12:
            if(!C2RXFUL1.rxful12)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf12;
               C2RXOVF1.rxovf12=0;
            }
            break;
         case 13:
            if(!C2RXFUL1.rxful13)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf13;
               C2RXOVF1.rxovf13=0;
            }
            break;
         case 14:
            if(!C2RXFUL1.rxful14)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf14;
               C2RXOVF1.rxovf14=0;
            }
            break;
         case 15:
            if(!C2RXFUL1.rxful15)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF1.rxovf15;
               C2RXOVF1.rxovf15=0;
            }
            break;
         case 16:
            if(!C2RXFUL2.rxful16)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf16;
               C2RXOVF2.rxovf16=0;
            }
            break;
         case 17:
            if(!C2RXFUL2.rxful17)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf17;
               C2RXOVF2.rxovf17=0;
            }
            break;
         case 18:
            if(!C2RXFUL2.rxful18)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf18;
               C2RXOVF2.rxovf18=0;
            }
            break;
         case 19:
            if(!C2RXFUL2.rxful19)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf19;
               C2RXOVF2.rxovf19=0;
            }
            break;
         case 20:
            if(!C2RXFUL2.rxful20)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf20;
               C2RXOVF2.rxovf20=0;
            }
            break;
         case 21:
            if(!C2RXFUL2.rxful21)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf21;
               C2RXOVF2.rxovf21=0;
            }
            break;
         case 22:
            if(!C2RXFUL2.rxful22)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf22;
               C2RXOVF2.rxovf22=0;
            }
            break;
         case 23:
            if(!C2RXFUL2.rxful23)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf23;
               C2RXOVF2.rxovf23=0;
            }
            break;
         case 24:
            if(!C2RXFUL2.rxful24)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf24;
               C2RXOVF2.rxovf24=0;
            }
            break;
         case 25:
            if(!C2RXFUL2.rxful25)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf25;
               C2RXOVF2.rxovf25=0;
            }
            break;
         case 26:
            if(!C2RXFUL2.rxful26)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf26;
               C2RXOVF2.rxovf26=0;
            }
            break;
         case 27:
            if(!C2RXFUL2.rxful27)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf27;
               C2RXOVF2.rxovf27=0;
            }
            break;
         case 28:
            if(!C2RXFUL2.rxful28)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf28;
               C2RXOVF2.rxovf28=0;
            }
            break;
         case 29:
            if(!C2RXFUL2.rxful29)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf29;
               C2RXOVF2.rxovf29=0;
            }
            break;
         case 30:
            if(!C2RXFUL2.rxful30)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf30;
               C2RXOVF2.rxovf30=0;
            }
            break;
         case 31:
            if(!C2RXFUL2.rxful31)
               return(0);
            else {
               stat.err_ovfl=C2RXOVF2.rxovf31;
               C2RXOVF2.rxovf31=0;
            }
            break;
      }
      
      C2INTF.rbif=0;
      
      stat.buffer=C2FIFO.fnrb;
      
      ptr=&ecan2_message_buffer[stat.buffer][0];
      stat.ext=(*ptr & 0x1);
      temp=(*ptr & 0x1);
        
      ptr+=2;
      len=(*ptr & 0xF);
      stat.rtr=((make8(*ptr,1) & 0x2) >> 1);
      
      ptr+=5;
      stat.filthit=(make8(*ptr,1) & 0x1F);
         
      id=can2_get_id(stat.buffer,temp);
      
      ptr=&ecan2_message_buffer[stat.buffer][3];
      for(i=0;i<len;i+=2)
      {
         data[i]=make8(*ptr,0);
         if (i+1!=len)
            data[i+1]=make8(*ptr,1);
         ptr++;
      }
      
      stat.inv=C2INTF.ivrif;
      C2INTF.ivrif=0;
      
      switch(stat.buffer)
      {
         case 0:
            C2RXFUL1.rxful0=0;
            break;
         case 1:
            C2RXFUL1.rxful1=0;
            break;
         case 2:
            C2RXFUL1.rxful2=0;
            break;
         case 3:
            C2RXFUL1.rxful3=0;
            break;
         case 4:
            C2RXFUL1.rxful4=0;
            break;
         case 5:
            C2RXFUL1.rxful5=0;
            break;
         case 6:
            C2RXFUL1.rxful6=0;
            break;
         case 7:
            C2RXFUL1.rxful7=0;
            break;
         case 8:
            C2RXFUL1.rxful8=0;
            break;
         case 9:
            C2RXFUL1.rxful9=0;
            break;
         case 10:
            C2RXFUL1.rxful10=0;
            break;
         case 11:
            C2RXFUL1.rxful11=0;
            break;
         case 12:
            C2RXFUL1.rxful12=0;
            break;
         case 13:
            C2RXFUL1.rxful13=0;
            break;
         case 14:
            C2RXFUL1.rxful14=0;
            break;
         case 15:
            C2RXFUL1.rxful15=0;
            break;
         case 16:
            C2RXFUL2.rxful16=0;
            break;
         case 17:
            C2RXFUL2.rxful17=0;
            break;
         case 18:
            C2RXFUL2.rxful18=0;
            break;
         case 19:
            C2RXFUL2.rxful19=0;
            break;
         case 20:
            C2RXFUL2.rxful20=0;
            break;
         case 21:
            C2RXFUL2.rxful21=0;
            break;
         case 22:
            C2RXFUL2.rxful22=0;
            break;
         case 23:
            C2RXFUL2.rxful23=0;
            break;
         case 24:
            C2RXFUL2.rxful24=0;
            break;
         case 25:
            C2RXFUL2.rxful25=0;
            break;
         case 26:
            C2RXFUL2.rxful26=0;
            break;
         case 27:
            C2RXFUL2.rxful27=0;
            break;
         case 28:
            C2RXFUL2.rxful28=0;
            break;
         case 29:
            C2RXFUL2.rxful29=0;
            break;
         case 30:
            C2RXFUL2.rxful30=0;
            break;
         case 31:
            C2RXFUL2.rxful31=0;
            break;
      }
      
      return(1);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_trb0_putd - can_trb7_putd
// can2_trb0_putd - can2_trb7_putd
//
// Places data to be transfered in a specified buffer
//
// Parameters:
//      id - id that will be sent with the data
//    data - pointer to the data
//      len - number of data bytes (0-8)
//      pri - priority (0-3)
//      ext - extended or not
//    rtr - request remote transfer
//
// Returns:
//      TRUE if data is succesfully loaded into the buffer
//      FALSE if data can not be loaded into the buffer
//
//   Notes
//      - make sure that the desired buffer is set to be a trasfer buffer
//        using the can_enable_b_transfer ( ) function
//
////////////////////////////////////////////////////////////////////////////////

// programmable transfer buffer TRB0
int1 can_trb0_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
{
   uint16_t i;
   uint16_t *ptr;
   uint16_t temp;
   
   if(C1TR01CON.txreqm || !C1TR01CON.txenm)
      return(0);
      
   C1TR01CON.txmpri=pri;
   
   if(ext)
   {
      temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
      ecan1_message_buffer[0][0]=temp;
      temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
      ecan1_message_buffer[0][1]=temp;
      temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
      ecan1_message_buffer[0][2]=temp;
   }
   else
   {
      temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
      ecan1_message_buffer[0][0]=temp;
      ecan1_message_buffer[0][1]=0x0000;
      temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
      ecan1_message_buffer[0][2]=temp;
   }
   
   ptr=&ecan1_message_buffer[0][3];
   for(i=0;i<len;i+=2)
   {
      if (i+1==len)
         *ptr=make16(0,data[i]);
      else
         *ptr=make16(data[i+1],data[i]);
      ptr++;
   }
   
   C1TR01CON.txreqm=1;
   
   return(1);
}

#if getenv("SFR:C2CTRL1")
   int1 can2_trb0_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
   {
      uint16_t i;
      uint16_t *ptr;
      uint16_t temp;
      
      if(C2TR01CON.txreqm || !C2TR01CON.txenm)
         return(0);
         
      C2TR01CON.txmpri=pri;
      
      if(ext)
      {
         temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan2_message_buffer[0][0]=temp;
         temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
         ecan2_message_buffer[0][1]=temp;
         temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
         ecan2_message_buffer[0][2]=temp;
      }
      else
      {
         temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan2_message_buffer[0][0]=temp;
         ecan2_message_buffer[0][1]=0x0000;
         temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
         ecan2_message_buffer[0][2]=temp;
      }
      
      ptr=&ecan2_message_buffer[0][3];
      for(i=0;i<len;i+=2)
      {
         if (i+1==len)
            *ptr=make16(0,data[i]);
         else
            *ptr=make16(data[i+1],data[i]);
         ptr++;
      }
      
      C2TR01CON.txreqm=1;
      
      return(1);
   }
#endif

// programmable transfer buffer TRB1
int1 can_trb1_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
{
   uint16_t i;
   uint16_t *ptr;
   uint16_t temp;
   
   if(C1TR01CON.txreqn || !C1TR01CON.txenn)
      return(0);
      
   C1TR01CON.txnpri=pri;
   
   if(ext)
   {
      temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
      ecan1_message_buffer[1][0]=temp;
      temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
      ecan1_message_buffer[1][1]=temp;
      temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
      ecan1_message_buffer[1][2]=temp;
   }
   else
   {  
      temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
      ecan1_message_buffer[1][0]=temp;
      ecan1_message_buffer[1][1]=0x0000;
      temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
      ecan1_message_buffer[1][2]=temp;
   }
   
   ptr=&ecan1_message_buffer[1][3];
   for(i=0;i<len;i+=2)
   {
      if (i+1==len)
         *ptr=make16(0,data[i]);
      else
         *ptr=make16(data[i+1],data[i]);
      ptr++;
   }
   
   C1TR01CON.txreqn=1;
   
   return(1);
}

#if getenv("SFR:C2CTRL1")
   int1 can2_trb1_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
   {
      uint16_t i;
      uint16_t *ptr;
      uint16_t temp;
      
      if(C2TR01CON.txreqn || !C2TR01CON.txenn)
         return(0);
         
      C2TR01CON.txnpri=pri;
      
      if(ext)
      {
         temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan2_message_buffer[1][0]=temp;
         temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
         ecan2_message_buffer[1][1]=temp;
         temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
         ecan2_message_buffer[1][2]=temp;
      }
      else
      {  
         temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan2_message_buffer[1][0]=temp;
         ecan2_message_buffer[1][1]=0x0000;
         temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
         ecan2_message_buffer[1][2]=temp;
      }
      
      ptr=&ecan2_message_buffer[1][3];
      for(i=0;i<len;i+=2)
      {
         if (i+1==len)
            *ptr=make16(0,data[i]);
         else
            *ptr=make16(data[i+1],data[i]);
         ptr++;
      }
      
      C2TR01CON.txreqn=1;
      
      return(1);
   }
#endif

// programmable transfer buffer TRB2
int1 can_trb2_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
{
   uint16_t i;
   uint16_t *ptr;
   uint16_t temp;
   
   if(C1TR23CON.txreqm || !C1TR23CON.txenm)
      return(0);
      
   C1TR23CON.txmpri=pri;
   
   if(ext)
   {
      temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
      ecan1_message_buffer[2][0]=temp;
      temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
      ecan1_message_buffer[2][1]=temp;
      temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
      ecan1_message_buffer[2][2]=temp;
   }
   else
   {
      temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
      ecan1_message_buffer[2][0]=temp;
      ecan1_message_buffer[2][1]=0x0000;
      temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
      ecan1_message_buffer[2][2]=temp;
   }
   
   ptr=&ecan1_message_buffer[2][3];
   for(i=0;i<len;i+=2)
   {
      if (i+1==len)
         *ptr=make16(0,data[i]);
      else
         *ptr=make16(data[i+1],data[i]);
      ptr++;
   }
   
   C1TR23CON.txreqm=1;
   
   return(1);
}

#if getenv("SFR:C2CTRL1")
   int1 can2_trb2_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
   {
      uint16_t i;
      uint16_t *ptr;
      uint16_t temp;
      
      if(C2TR23CON.txreqm || !C2TR23CON.txenm)
         return(0);
         
      C2TR23CON.txmpri=pri;
      
      if(ext)
      {
         temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan2_message_buffer[2][0]=temp;
         temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
         ecan2_message_buffer[2][1]=temp;
         temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
         ecan2_message_buffer[2][2]=temp;
      }
      else
      {
         temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan2_message_buffer[2][0]=temp;
         ecan2_message_buffer[2][1]=0x0000;
         temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
         ecan2_message_buffer[2][2]=temp;
      }
      
      ptr=&ecan2_message_buffer[2][3];
      for(i=0;i<len;i+=2)
      {
         if (i+1==len)
            *ptr=make16(0,data[i]);
         else
            *ptr=make16(data[i+1],data[i]);
         ptr++;
      }
      
      C2TR23CON.txreqm=1;
      
      return(1);
   }
#endif

// programmable transfer buffer TRB3
int1 can_trb3_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
{
   uint16_t i;
   uint16_t *ptr;
   uint16_t temp;
   
   if(C1TR23CON.txreqn || !C1TR23CON.txenn)
      return(0);
      
   C1TR23CON.txnpri=pri;
   
   if(ext)
   {
      temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
      ecan1_message_buffer[3][0]=temp;
      temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
      ecan1_message_buffer[3][1]=temp;
      temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
      ecan1_message_buffer[3][2]=temp;
   }
   else
   {
      temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
      ecan1_message_buffer[3][0]=temp;
      ecan1_message_buffer[3][1]=0x0000;
      temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
      ecan1_message_buffer[3][2]=temp;
   }
   
   ptr=&ecan1_message_buffer[3][3];
   for(i=0;i<len;i+=2)
   {
      if (i+1==len)
         *ptr=make16(0,data[i]);
      else
         *ptr=make16(data[i+1],data[i]);
      ptr++;
   }
   
   C1TR23CON.txreqn=1;
   
   return(1);
}

#if getenv("SFR:C2CTRL1")
   int1 can2_trb3_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
   {
      uint16_t i;
      uint16_t *ptr;
      uint16_t temp;
      
      if(C2TR23CON.txreqn || !C2TR23CON.txenn)
         return(0);
         
      C2TR23CON.txnpri=pri;
      
      if(ext)
      {
         temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan2_message_buffer[3][0]=temp;
         temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
         ecan2_message_buffer[3][1]=temp;
         temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
         ecan2_message_buffer[3][2]=temp;
      }
      else
      {
         temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan2_message_buffer[3][0]=temp;
         ecan2_message_buffer[3][1]=0x0000;
         temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
         ecan2_message_buffer[3][2]=temp;
      }
      
      ptr=&ecan2_message_buffer[3][3];
      for(i=0;i<len;i+=2)
      {
         if (i+1==len)
            *ptr=make16(0,data[i]);
         else
            *ptr=make16(data[i+1],data[i]);
         ptr++;
      }
      
      C2TR23CON.txreqn=1;
      
      return(1);
   }
#endif

// programmable transfer buffer TRB4
#if CAN_BUFFER_SIZE>4
   int1 can_trb4_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
   {
      uint16_t i;
      uint16_t *ptr;
      uint16_t temp;
      
      if(C1TR45CON.txreqm || !C1TR45CON.txenm)
         return(0);
         
      C1TR45CON.txmpri=pri;
      
      if(ext)
      {
         temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan1_message_buffer[4][0]=temp;
         temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
         ecan1_message_buffer[4][1]=temp;
         temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
         ecan1_message_buffer[4][2]=temp;
      }
      else
      {
         temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan1_message_buffer[4][0]=temp;
         ecan1_message_buffer[4][1]=0x0000;
         temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
         ecan1_message_buffer[4][2]=temp;
      }
      
      ptr=&ecan1_message_buffer[4][3];
      for(i=0;i<len;i+=2)
      {
         if (i+1==len)
            *ptr=make16(0,data[i]);
         else
            *ptr=make16(data[i+1],data[i]);
         ptr++;
      }
      
      C1TR45CON.txreqm=1;
      
      return(1);
   }
#endif

#if getenv("SFR:C2CTRL1")
   #if CAN2_BUFFER_SIZE>4
      int1 can2_trb4_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
      {
         uint16_t i;
         uint16_t *ptr;
         uint16_t temp;
         
         if(C2TR45CON.txreqm || !C2TR45CON.txenm)
            return(0);
            
         C2TR45CON.txmpri=pri;
         
         if(ext)
         {
            temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
            ecan2_message_buffer[4][0]=temp;
            temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
            ecan2_message_buffer[4][1]=temp;
            temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
            ecan2_message_buffer[4][2]=temp;
         }
         else
         {
            temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
            ecan2_message_buffer[4][0]=temp;
            ecan2_message_buffer[4][1]=0x0000;
            temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
            ecan2_message_buffer[4][2]=temp;
         }
         
         ptr=&ecan2_message_buffer[4][3];
         for(i=0;i<len;i+=2)
         {
            if (i+1==len)
               *ptr=make16(0,data[i]);
            else
               *ptr=make16(data[i+1],data[i]);
            ptr++;
         }
         
         C2TR45CON.txreqm=1;
         
         return(1);
      }
   #endif
#endif

// programmable transfer buffer TRB5
#if CAN_BUFFER_SIZE>4
   int1 can_trb5_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
   {
      uint16_t i;
      uint16_t *ptr;
      uint16_t temp;
      
      if(C1TR45CON.txreqn || !C1TR45CON.txenn)
         return(0);
         
      C1TR45CON.txnpri=pri;
      
      if(ext)
      {
         temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan1_message_buffer[5][0]=temp;
         temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
         ecan1_message_buffer[5][1]=temp;
         temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
         ecan1_message_buffer[5][2]=temp;
      }
      else
      {
         temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan1_message_buffer[5][0]=temp;
         ecan1_message_buffer[5][1]=0x0000;
         temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
         ecan1_message_buffer[5][2]=temp;
      }
      
      ptr=&ecan1_message_buffer[5][3];
      for(i=0;i<len;i+=2)
      {
         if (i+1==len)
            *ptr=make16(0,data[i]);
         else
            *ptr=make16(data[i+1],data[i]);
         ptr++;
      }
      
      C1TR45CON.txreqn=1;
      
      return(1);
   }
#endif

#if getenv("SFR:C2CTRL1")
   #if CAN2_BUFFER_SIZE>4
      int1 can2_trb5_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
      {
         uint16_t i;
         uint16_t *ptr;
         uint16_t temp;
         
         if(C2TR45CON.txreqn || !C2TR45CON.txenn)
            return(0);
            
         C2TR45CON.txnpri=pri;
         
         if(ext)
         {
            temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
            ecan2_message_buffer[5][0]=temp;
            temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
            ecan2_message_buffer[5][1]=temp;
            temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
            ecan2_message_buffer[5][2]=temp;
         }
         else
         {
            temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
            ecan2_message_buffer[5][0]=temp;
            ecan2_message_buffer[5][1]=0x0000;
            temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
            ecan2_message_buffer[5][2]=temp;
         }
         
         ptr=&ecan2_message_buffer[5][3];
         for(i=0;i<len;i+=2)
         {
            if (i+1==len)
               *ptr=make16(0,data[i]);
            else
               *ptr=make16(data[i+1],data[i]);
            ptr++;
         }
         
         C2TR45CON.txreqn=1;
         
         return(1);
      }
   #endif
#endif
   
// programmable transfer buffer TRB6
#if CAN_BUFFER_SIZE>6
   int1 can_trb6_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
   {
      uint16_t i;
      uint16_t *ptr;
      uint16_t temp;
      
      if(C1TR67CON.txreqm || !C1TR67CON.txenm)
         return(0);
         
      C1TR67CON.txmpri=pri;
      
      if(ext)
      {
         temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan1_message_buffer[6][0]=temp;
         temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
         ecan1_message_buffer[6][1]=temp;
         temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
         ecan1_message_buffer[6][2]=temp;
      }
      else
      {
         temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan1_message_buffer[6][0]=temp;
         ecan1_message_buffer[6][1]=0x0000;
         temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
         ecan1_message_buffer[6][2]=temp;
      }
      
      ptr=&ecan1_message_buffer[6][3];
      for(i=0;i<len;i+=2)
      {
         if (i+1==len)
            *ptr=make16(0,data[i]);
         else
            *ptr=make16(data[i+1],data[i]);
         ptr++;
      }
      
      C1TR67CON.txreqm=1;
      
      return(1);
   }
#endif

#if getenv("SFR:C2CTRL1")
   #if CAN2_BUFFER_SIZE>6
      int1 can2_trb6_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
      {
         uint16_t i;
         uint16_t *ptr;
         uint16_t temp;
         
         if(C2TR67CON.txreqm || !C2TR67CON.txenm)
            return(0);
            
         C2TR67CON.txmpri=pri;
         
         if(ext)
         {
            temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
            ecan2_message_buffer[6][0]=temp;
            temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
            ecan2_message_buffer[6][1]=temp;
            temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
            ecan2_message_buffer[6][2]=temp;
         }
         else
         {
            temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
            ecan2_message_buffer[6][0]=temp;
            ecan2_message_buffer[6][1]=0x0000;
            temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
            ecan2_message_buffer[6][2]=temp;
         }
         
         ptr=&ecan2_message_buffer[6][3];
         for(i=0;i<len;i+=2)
         {
            if (i+1==len)
               *ptr=make16(0,data[i]);
            else
               *ptr=make16(data[i+1],data[i]);
            ptr++;
         }
         
         C2TR67CON.txreqm=1;
         
         return(1);
      }
   #endif
#endif

// programmable transfer buffer TRB7
#if CAN_BUFFER_SIZE>6
   int1 can_trb7_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
   {
      uint16_t i;
      uint16_t *ptr;
      uint16_t temp;
      
      if(C1TR67CON.txreqn || !C1TR67CON.txenn)
         return(0);
      
      C1TR67CON.txnpri=pri;
      
      if(ext)
      {
         temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan1_message_buffer[7][0]=temp;
         temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
         ecan1_message_buffer[7][1]=temp;
         temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
         ecan1_message_buffer[7][2]=temp;
      }
      else
      {
         temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
         ecan1_message_buffer[7][0]=temp;
         ecan1_message_buffer[7][1]=0x0000;
         temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
         ecan1_message_buffer[7][2]=temp;
      }
      
      ptr=&ecan1_message_buffer[7][3];
      for(i=0;i<len;i+=2)
      {
         if (i+1==len)
            *ptr=make16(0,data[i]);
         else
            *ptr=make16(data[i+1],data[i]);
         ptr++;
      }
      
      C1TR67CON.txreqn=1;
      
      return(1);
   }
#endif

#if getenv("SFR:C2CTRL1")
   #if CAN2_BUFFER_SIZE>6
      int1 can2_trb7_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t pri, int1 ext, int1 rtr)
      {
         uint16_t i;
         uint16_t *ptr;
         uint16_t temp;
         
         if(C2TR67CON.txreqn || !C2TR67CON.txenn)
            return(0);
         
         C2TR67CON.txnpri=pri;
         
         if(ext)
         {
            temp=(((id & 0x1FFC0000) >> 16) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
            ecan2_message_buffer[7][0]=temp;
            temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
            ecan2_message_buffer[7][1]=temp;
            temp=(((id & 0x3F) << 10) | (((uint16_t)rtr << 9) | len)) & 0xFF1F;
            ecan2_message_buffer[7][2]=temp;
         }
         else
         {
            temp=(((id & 0x7FF) << 2) | (((uint16_t)rtr << 1) | ext)) & 0x1FFF;
            ecan2_message_buffer[7][0]=temp;
            ecan2_message_buffer[7][1]=0x0000;
            temp=(((uint16_t)rtr << 9) | len)& 0xFF1F;
            ecan2_message_buffer[7][2]=temp;
         }
         
         ptr=&ecan2_message_buffer[7][3];
         for(i=0;i<len;i+=2)
         {
            if (i+1==len)
               *ptr=make16(0,data[i]);
            else
               *ptr=make16(data[i+1],data[i]);
            ptr++;
         }
         
         C2TR67CON.txreqn=1;
         
         return(1);
      }
   #endif
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_config_DMA()
// can2_config_DMA()
//
// Configures the DMA buffer to use the ECAN Peripheral
//    Transmitt CAN1 set to DMA Channel 0
//    Receive CAN1 set to DMA Channel 1
//    Transmitt CAN2 set to DMA Channel 2
//    Receive CAN2 set to DMA Channel 3
//       
////////////////////////////////////////////////////////////////////////////////
void can_config_DMA(void)
{
   // transmission
   DMA0CON.size=0;
   DMA0CON.dir=1;
   DMA0CON.amode=PERIPHERAL_INDIRECT;
   DMA0CON.mode=CONTINUOUS_PINGPONG_DISABLED;
   DMA0REQ.irqsel=ECAN1_TRANSMISSION;
   DMA0CNT=7;
   DMA0PAD=&C1TXD;
   DMA0STA=&ecan1_message_buffer[0][0]-0x4000;
   DMA0CON.chen=1;
   
   // receive
   DMA1CON.size=0;
   DMA1CON.dir=0;
   DMA1CON.amode=PERIPHERAL_INDIRECT;
   DMA1CON.mode=CONTINUOUS_PINGPONG_DISABLED;
   DMA1REQ.irqsel=ECAN1_RECEPTION;
   DMA1CNT=7;
   DMA1PAD=&C1RXD;
   DMA1STA=&ecan1_message_buffer[0][0];
   DMA1CON.chen=1;
}

#if getenv("SFR:C2CTRL1")
   void can2_config_DMA(void)
   {
      // transmission
      DMA2CON.size=0;
      DMA2CON.dir=1;
      DMA2CON.amode=PERIPHERAL_INDIRECT;
      DMA2CON.mode=CONTINUOUS_PINGPONG_DISABLED;
      DMA2REQ.irqsel=ECAN2_TRANSMISSION;
      DMA2CNT=7;
      DMA2PAD=&C2TXD;
      DMA2STA=&ecan2_message_buffer[0][0];
      DMA2CON.chen=1;
      
      // receive
      DMA3CON.size=0;
      DMA3CON.dir=0;
      DMA3CON.amode=PERIPHERAL_INDIRECT;
      DMA3CON.mode=CONTINUOUS_PINGPONG_DISABLED;
      DMA3REQ.irqsel=ECAN2_RECEPTION;
      DMA3CNT=7;
      DMA3PAD=&C2RXD;
      DMA3STA=&ecan2_message_buffer[0][0]-0x4000;
      DMA3CON.chen=1;
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_enable_interrupts()
// can2_enable_interrupts()
//
//    Selects which interrupts causes the #INT_CAN1 and #INT_CAN2 interrupts to 
//    fire
//
//    Parameters
//       setting - the selected interrupts 
//          enumerated as
//             TB - Transmitt Buffer Interrupt
//             RB - Receive Buffer Interrupt
//             RXOV - Receive Buffer Overflow Interrupt
//             FIFO - FIFO Almost Full Interrupt
//             ERR - Error interrupt
//             WAK - Wake-Up Interrupt
//             IVR - Invalid Message Received Interrupt
//
//          You can or these together (via |) to enable more than interrupt
//
////////////////////////////////////////////////////////////////////////////////
void can_enable_interrupts(INTERRUPT setting)
{
   uint16_t temp;
   uint16_t *ptr;
   
   ptr = &C1INTE;
   
  // temp=C1INTE;
  // C1INTE=(temp | setting);
  
  temp = *ptr;
  *ptr = (temp | setting);
}

#if getenv("SFR:C2CTRL1")
   void can2_enable_interrupts(INTERRUPT setting)
   {
      uint16_t temp;
      uint16_t *ptr;
      
      ptr = &C2INTE;
      
      //temp=C2INTE;
      //C2INTE=(temp | setting);
      temp = *ptr;
      *ptr = (temp | setting);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_disable_interrupts()
// can2_disable_interrupts()
//
//    Disables specifed interupts that causes the #INT_CAN1 and #INT_CAN2
//       interrupts to fire
//
//    Parameters
//       setting - the selected interrupts 
//          enumerated as
//             TB - Transmitt Buffer Interrupt
//             RB - Receive Buffer Interrupt
//             RXOV - Receive Buffer Overflow Interrupt
//             FIFO - FIFO Almost Full Interrupt
//             ERR - Error interrupt
//             WAK - Wake-Up Interrupt
//             IVR - Invalid Message Received Interrupt
//
//          You can or these together (via |) to enable more than interrupt
//
////////////////////////////////////////////////////////////////////////////////
void can_disable_interrupts(INTERRUPT setting)
{
   uint16_t temp;
   uint16_t *ptr;
   
   ptr = &C1INTE;
   
   //temp=C1INTE;
   temp = *ptr;
   temp&=(setting^0xFF);
   
   //C1INTE=temp;
   *ptr = temp;
}

#if getenv("SFR:C2CTRL1")
   void can2_disable_interrupts(INTERRUPT setting)
   {
      uint16_t temp;
      uint16_t ptr;
      
      ptr = &C2INTE;
      
      //temp=C2INTE;
      temp = *ptr;
      temp&=(setting^0xFF);
      
      //C2INTE=temp;
      *ptr = temp;
   }
#endif
