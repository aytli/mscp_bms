/////////////////////////////////////////////////////////////////////////
////                        can-18F4580.c                            ////
//// CAN Library routines for Microchip's PIC18Cxx8 and 18Fxx8 line  ////
////                                                                 ////
//// This library provides the following functions:                  ////
////  (for more information on these functions see the comment       ////
////   header above each function)                                   ////
////                                                                 ////
////    can_init - Configures the PIC18F4580 CAN peripheral*         ////
////                                                                 ////
////    can_set_baud - Sets the baud rate control registers*         ////
////                                                                 ////
////    can_set_mode - Sets the CAN module into a specific mode*     ////
////                                                                 ////
////     can_set_functional_mode - Sets the function mode            ////
////                                                                 ////
////    can_set_id - Sets the standard and extended ID*              ////
////                                                                 ////
////     can_set_extended_id - Sets only extended ID                 ////
////                                                                 ////
////     can_set_standard_id - Sets only standard ID                 ////
////                                                                 ////
////    can_get_id - Gets the standard and extended ID*              ////
////                                                                 ////
////     can_get_extended_id - Gets only extended ID                 ////
////                                                                 ////
////     can_get_standard_id - Gets only standard ID                 ////
////                                                                 ////
////    can_putd - Sends a message/request with specified ID*        ////
////                                                                 ////
////    can_getd - Returns specifid message/request and ID*          ////
////                                                                 ////
////    can_ - Returns true if there is data in one of the*     ////
////                receive buffers                                  ////
////                                                                 ////
////    can_tbe - Returns true if the transmit buffer is ready to    ////
////              send more data*                                    ////
////                                                                 ////
////    can_abort - Aborts all pending transmissions*                ////
////                                                                 ////
////    can_enable_b_transfer - enables buffer as transmitter        ////
////                                                                 ////
////     can_enable_b_receiver - enables buffer as receiver          ////
////                                                                 ////
////     can_enable_rtr - enables transmit buffer as Auto-rtr        ////
////                                                                 ////
////     can_disable_rtr - disables transmit buffer as Auto-rtr      ////
////                                                                 ////
////    can_load_rtr - loads an Auto-rtr buffer with data            ////
////                                                                 ////
////    can_enable_filter - enables one of 15 filters                ////
////                                                                 ////
////    can_disable_filter - disables one of the 15 filters          ////
////                                                                 ////
////    can_associate_filter_to_buffer - associates a filter with    ////
////                                      a receive buffer           ////
////                                                                 ////
////     can_associate_filter_to_mask - associates a filter with a   ////
////                                    mask                         ////
////                                                                 ////
////     can_fifo_getd - retrive data in FIFO mode (2)               ////
////                                                                 ////
////     can_t0_putd                                                 ////
////     can_t1_putd                                                 ////
////     can_t2_putd                                                 ////
////     can_b0_putd                                                 ////
////     can_b1_putd                                                 ////
////     can_b2_putd                                                 ////
////     can_b3_putd                                                 ////
////     can_b4_putd                                                 ////
////     can_b5_putd                                                 ////
////     can_b6_putd                                                 ////
////     can_b7_putd - place data in a specific buffer               ////
////                                                                 ////
//// PIN_B3 is CANRX, and PIN_B2 is CANTX.  You will need a CAN      ////
//// transeiver to connect these pins to CANH and CANL bus lines.    ////
////                                                                 ////
//// CCS provides an example, ex_can.c, which shows how to use this  ////
//// library.                                                        ////
////                                                                 ////
//// * Designates functions that work in legacy mode                 ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Version History                                                 ////
////                                                                 ////
////  Jul 27 04 - can_init() uses CAN_USE_EXTENDED_ID instead of     ////
////              setting all RX filters to extended.                ////
////                                                                 ////
////  Feb 24 04 - can_get_id() fixed for EID<18:20>.                 ////
////                                                                 ////
////  Mar 30 09 - added CANTX2 code for PIC18F6585/8585/6680/8680    ////
////                                                                 ////
////  Sep 29 09 - fixed problems with enhanced mode                  ////
////                                                                 ////
////  Oct 08 09 - can_getd() filter hit fixed of enhanced mode       ////
////                                                                 ////
////  Jan 21 10 - changed variable types to unsigned incase compiled ////
////              with #device ANSI, which makes variables signed    ////
////                                                                 ////
////  Apr 13 10 - changed can_set_functional_mode() to restore       ////
////              operating mode to same mode when called, instead   ////
////              of setting it to normal mode                       ////
////                                                                 ////
////  Mar 24 11 - updated for new PIC18FxxK80 chips                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2011 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <can-18F4580_mscp.h>

#if CAN_DO_DEBUG
 #use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
 #define can_debug printf
#else
 #define can_debug
#endif

//macros
#define can_kbhit() (RXB0CON.rxful || RXB1CON.rxful || (B0CONR.rxful && !BSEL0.b0txen) || (B1CONR.rxful && !BSEL0.b1txen) || (B2CONR.rxful && !BSEL0.b2txen) || (B3CONR.rxful && !BSEL0.b3txen) || (B4CONR.rxful && !BSEL0.b4txen) || (B5CONR.rxful && !BSEL0.b5txen))
#define can_tbe() (!TXB0CON.txreq || !TXB1CON.txreq || !TXB2CON.txreq || (!B0CONT.txreq && BSEL0.b0txen) || (!B1CONT.txreq && BSEL0.b1txen) || (!B2CONT.txreq && BSEL0.b2txen) || (!B3CONT.txreq && BSEL0.b3txen) || (!B4CONT.txreq && BSEL0.b4txen) || (!B5CONT.txreq && BSEL0.b5txen))
#define can_abort()                 (CANCON.abat=1)

// current mode variable
// used by many of the device drivers to prevent damage from the mode
//
unsigned int curmode;
unsigned int curfunmode;

////////////////////////////////////////////////////////////////////////
//
// can_init()
//
// Initializes PIC18xxx8 CAN peripheral.  Sets the RX filter and masks so the
// CAN peripheral will receive all incoming IDs.  Configures both RX buffers
// to only accept valid valid messages (as opposed to all messages, or all
// extended message, or all standard messages).  Also sets the tri-state
// setting of B2 to output, and B3 to input (apparently the CAN peripheral
// doesn't keep track of this)
//
// The constants (CAN_USE_RX_DOUBLE_BUFFER, CAN_ENABLE_DRIVE_HIGH,
// CAN_ENABLE_CAN_CAPTURE) are given a default define in the can-18xxx8.h file.
// These default values can be overwritten in the main code, but most
// applications will be fine with these defaults.
//
//////////////////////////////////////////////////////////////////////////////
void can_init(void) {
   can_set_mode(CAN_OP_CONFIG);   //must be in config mode before params can be set
   can_set_baud();
   curfunmode=CAN_FUN_OP_LEGACY;

   // RXB0CON
   //    filthit0=0
   //    jtoff=0
   //      rxb0dben=1   buffer zero will overflow into buffer one
   //      rxrtrro=0
   //      rxm1:0=0      will recive all valid IDs
   RXB0CON=0;
   RXB0CON.rxm=CAN_RX_VALID;
   RXB0CON.rxb0dben=CAN_USE_RX_DOUBLE_BUFFER;
   RXB1CON=RXB0CON;

   CIOCON.endrhi=CAN_ENABLE_DRIVE_HIGH;
   CIOCON.cancap=CAN_ENABLE_CAN_CAPTURE;
   CIOCON.tx2src=CAN_CANTX2_SOURCE;       //added for PIC18F6585/8585/6680/8680
   CIOCON.tx2en=CAN_ENABLE_CANTX2;        //added for PIC18F6585/8585/6680/8680

   can_set_id(RX0MASK, CAN_MASK_ACCEPT_ALL, CAN_USE_EXTENDED_ID);  //set mask 0
   can_set_id(RXFILTER0, 0, CAN_USE_EXTENDED_ID);  //set filter 0 of mask 0
   can_set_id(RXFILTER1, 0, CAN_USE_EXTENDED_ID);  //set filter 1 of mask 0

   can_set_id(RX1MASK, CAN_MASK_ACCEPT_ALL, CAN_USE_EXTENDED_ID);  //set mask 1
   can_set_id(RXFILTER2, 0, CAN_USE_EXTENDED_ID);  //set filter 0 of mask 1
   can_set_id(RXFILTER3, 0, CAN_USE_EXTENDED_ID);  //set filter 1 of mask 1
   can_set_id(RXFILTER4, 0, CAN_USE_EXTENDED_ID);  //set filter 2 of mask 1
   can_set_id(RXFILTER5, 0, CAN_USE_EXTENDED_ID);  //set filter 3 of mask 1

   // set dynamic filters
   can_set_id(RXFILTER6, 0, CAN_USE_EXTENDED_ID);
   can_set_id(RXFILTER7, 0, CAN_USE_EXTENDED_ID);
   can_set_id(RXFILTER8, 0, CAN_USE_EXTENDED_ID);
   can_set_id(RXFILTER9, 0, CAN_USE_EXTENDED_ID);
   can_set_id(RXFILTER10, 0, CAN_USE_EXTENDED_ID);
   can_set_id(RXFILTER11, 0, CAN_USE_EXTENDED_ID);
   can_set_id(RXFILTER12, 0, CAN_USE_EXTENDED_ID);
   can_set_id(RXFILTER13, 0, CAN_USE_EXTENDED_ID);
   can_set_id(RXFILTER14, 0, CAN_USE_EXTENDED_ID);
   can_set_id(RXFILTER15, 0, CAN_USE_EXTENDED_ID);
 
   can_set_mode(CAN_OP_NORMAL);
   curfunmode=CAN_FUN_OP_ENHANCED_FIFO;
   can_set_functional_mode(curfunmode);
}

////////////////////////////////////////////////////////////////////////
//
// can_set_baud()
//
// Configures the baud rate control registers.  All the defines here
// are defaulted in the can-18xxx8.h file.  These defaults can, and
// probably should, be overwritten in the main code.
//
// Current defaults are set to work with Microchip's MCP250xxx CAN
// Developers Kit if this PIC is running at 20Mhz.
//
//   BRGCON1 contains the prescaler bits and the Synchronization jump
//         width time bits.
//
//           the prescale values are
//              111111=(2*64)/clock=Tq
//              111110=(2*63)/clock=Tq
//                continued
//              000001=(2*2)/clock=Tq
//                000000=(2*1)/clock=Tq
//
//           in the case of can-18xxx8.h, the prescale bits are set to
//           000100=10/clock profided that the user does not define it
//           differently
//
//           The Synchronized Jump Width Bits are
//              11=4*Tq
//              10=3*Tq
//              01=2*Tq
//            00=1*Tq
//
//           in the case of can-18xxx8.h, the SJW bits are set to 0 or 1*Tq
//
//   BRGCON2 contains the Phase Segment 2 Time Select bit, the SAMple bit
//          the Phase Segment 1 bits, and the Propagation Time Select bits
//
//           SEG2PHTS
//                1=Freely Programmable
//              0=Maximum of PHEG1 or IPT, which ever is greatest
//
//           in the case of can-18xxx8.h, the SEG2PHTS bit is set to 1 for
//           freely programmable
//
//           SAM
//              1=Three Samples
//              0=One Sample
//
//           in the case of can-18xxx8.h, the SAM bit is set to 0 for
//           one sample
//
//           SEG1PH2:SEG1PH0
//              Phase Segment 1 = (SEG1PH2:SEG1PH0+1)*Tq
//
//           in the case of can-18xxx8.h, the SEG1PH2:SEG1PH0 bits are set to 5
//         for 6*Tq Phase Segment 1 Time
//
//           PRSEG2:PRSEG0
//              Propogation Time = (PRSEG2:PRSEG0+1)*TQ
//
//           in the case of can-18xxx8.h, the PRSEG2:PRSEG0 bits are set to 2
//           for 3*Tq Propogation Time
//
// BRGCON3 contains the WAKFIL bit and the Phase Segment 2 Time Select bits
//
//           WAKEFIL
//            1=CAN bus line filter is used for wake-up
//              0=CAN bus line filter is not used for wake-up
//
//           in the case of can-18xx8.h, the WAKEFIL bit is set to 0 for
//           CAN bus not used for wake-up
//
//           SEG2PH2:SEG2PH0
//              Phase Segment 2 Time = (SEG2PH2:SEG2PH0+1)*Tq
//
//           in the case of can-18xxx8.h, SEG2PH2:SEG3PH0 is set to 5 for
//         6*Tq Phase Segment 2 Time
//
// More information can be found in the PIC18F4580 datasheet section 23.9
////////////////////////////////////////////////////////////////////////
void can_set_baud(void) {
   BRGCON1.brp=CAN_BRG_PRESCALAR;
   BRGCON1.sjw=CAN_BRG_SYNCH_JUMP_WIDTH;

   BRGCON2.prseg=CAN_BRG_PROPAGATION_TIME;
   BRGCON2.seg1ph=CAN_BRG_PHASE_SEGMENT_1;
   BRGCON2.sam=CAN_BRG_SAM;
   BRGCON2.seg2phts=CAN_BRG_SEG_2_PHASE_TS;

   BRGCON3.seg2ph=CAN_BRG_PHASE_SEGMENT_2;
   BRGCON3.wakfil=CAN_BRG_WAKE_FILTER;
}


////////////////////////////////////////////////////////////////////////
//
// can_set_mode
//
// This function first sets three most significant bits (reqop2:reqop0)
// to the mode given through the argument.  The modes can be
//
// mode
//
// 1xx  -> Configuration Mode
//             This mode allows the user to configure the Control Registers,
//            the Acceptance Mask Registers and the Acceptance Filter
//            Registers.
//
// 011  -> Listen Only Mode
//            This mode allows the PIC microcontroller to listen to the
//            bus but not actually write to it.
//
// 010  -> Loop Back Mode
//            This mode allows to the transmission of data directly from
//            the Transmission Buffers to the Recive Buffers without
//            actually driving any data to the bus
//
// 001  -> Dissable Mode
//            This mode prefents the PIC microcontroller from transmitting
//            and reciving, however the WAKIF interrupt is still active
//
// 000  -> Normal Mode
//            This is the normal mode of opperation for the CAN bus.  It
//            will recive all messages and is the only mode in which
//            data can be transmitted.
//
//   The reqop bits do not immediatly change the mode of operation, the
// three most significant bits in the CANSTAT register (opmode2:opmode0)
// must change to reflect the actuall change in mode, therefore a while
// statement is used to check if the CANSTAT opmode bits have changed to
// reflect the passed in mode
//
// More information can be found in the PIC18F4580 datasheet section 23.3
////////////////////////////////////////////////////////////////////////
void can_set_mode(CAN_OP_MODE mode) {
   CANCON.reqop=mode;
   while( (CANSTAT.opmode) != mode );
}

////////////////////////////////////////////////////////////////////////
//
// can_set_functional_mode
//
// This function sets the functional mode of the PIC18F4580 ECAN modual
//
//   mode
//
// 00    -> mode 0 or Legacy Mode.  The processor defaults to this mode
//            on reset in order to maintain backwards compatibility will
//            the previouse PIC18Cxx8 and PIC18Fxx8 CAN moduals.  All code
//            written for the listed microcontrollers will work on the
//            PIC18F4580 will no alterations.
//
//            This mode provides the exact same registers as the previouse
//            moduals.  These include
//
//            3 Transmit Buffers: TXB0, TXB1, TXB2
//            2 Recive Buffers: RXB0, RXB1
//            2 Acceptance Masks: RXM0, RXM1
//            6 Acceptance Filters: RXF0, RXF1, RXF2, RXF3, RXF4, RXF5
//
// 01  -> mode 1 or Enhanced Legacy Mode. This mode is the same as mode
//            0 with the exception that there are more resources available
//            to the CAN modual.  These include
//
//            3  Transmit Buffers: TXB0, TXB1, TXB2
//            2  Recive Buffers: RXB0, RXB1
//            6  Programmable Buffers: B0, B1, B2, B3, B4, B5
//               Automatic RTR Handling on: B0, B1, B2, B3, B4, B5
//            16 Dynamically assigned acceptance filters: RXF0-RXF15
//            2  Dedicated mask registers,RXM0: RXM1
//            1  Programmable mask register: RXF15
//               Programmable data filter on standard identifier messages: SDFLC
//
// 10    -> mode 2 or Enhanced FIFO Mode.  This mode produces a first in
//            first out (FIFO) CAN data buffer.  This buffer can be from 2 too
//            8 bytes in length depending on which B buffers are used as recive
//            buffers and which are used as transmit buffers.  The first transmit
//            buffer defines the size, therefor if B4 was the first transmit
//            buffer, RXB0, BXB1, B0, B1, B2, and B3 would form the FIFO buffer
//            for a size of 6 bytes.  If B0 through B5 are all set to recive data,
//            the size will be 8 bytes.  If B0 is set to transmit, the size will
//            be 2 bytes.  The available resources include
//
//            3  Transmit Buffers: TXB0, TXB1, TXB2
//            2  Recive Buffers: RXB0, RXB1
//            6  Programmable Buffers: B0, B1, B2, B3, B4, B5
//               Automatic RTR Handling on: B0, B1, B2, B3, B4, B5
//            16 Acceptance Filters: RXF0-RXF15
//            2  Dedicated Mask Registers: RXM0, RXM1
//            1  Programmable mask register: RXF15
//               Programmable data filter on standard identifier messages: SDFLC
//
//   More information can be found in the PIC18F4580 datasheet section 23.4
////////////////////////////////////////////////////////////////////////////////
void can_set_functional_mode(CAN_FUN_OP_MODE mode)
{
   curmode=CANSTAT.opmode;

   can_set_mode(CAN_OP_CONFIG);   //must be in config mode before params can be set
   ECANCON.mdsel=mode;
   curfunmode=mode;
   
   can_set_mode(curmode);
}

////////////////////////////////////////////////////////////////////////
//
// can_set_id()
//
// Configures the xxxxEIDL, xxxxEIDH, xxxxSIDL and xxxxSIDH registers to
// configure the defined buffer to use the specified ID
//
//   Paramaters:
//     addr - pointer to first byte of ID register, starting with xxxxEIDL.
//            For example, a pointer to RXM1EIDL
//
//     id - ID to set buffer to
//     ext - Set to TRUE if this buffer uses an extended ID, FALSE if not
//
////////////////////////////////////////////////////////////////////////
void can_set_id(int8 *addr, int32 id, int1 ext) {
   //int *ptr;

   //ptr=addr;

   if (ext) {  //extended
      //eidl
      *addr=make8(id,0); //0:7

      //eidh
      addr--;
      *addr=make8(id,1); //8:15

      //sidl
      addr--;
      *addr=make8(id,2) & 0x03;   //16:17
      *addr|=(make8(id,2) << 3) & 0xE0; //18:20
      *addr|=0x08;


      //sidh
      addr--;
      *addr=((make8(id,2) >> 5) & 0x07 ); //21:23
      *addr|=((make8(id,3) << 3) & 0xF8);//24:28
   }
   else {   //standard
      //eidl
      *addr=0;

      //eidh
      addr--;
      *addr=0;

      //sidl
      addr--;
      *addr=(make8(id,0) << 5) & 0xE0;

      //sidh
      addr--;
      *addr=(make8(id,0) >> 3) & 0x1F;
      *addr|=(make8(id,1) << 5) & 0xE0;
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// can_set_standard_id
//
// sets a standard id
//
// Parameters:
//      addr - the address that is to be set to the id
//      id - the actual id
//
////////////////////////////////////////////////////////////////////////////////

void can_set_standard_id(int8 *addr, int32 id)
{
   //eidl
   *addr=0;

   //eidh
   addr--;
   *addr=0;

   //sidl
   addr--;
   *addr=(make8(id,0) << 5) & 0xE0;

   //sidh
   addr--;
   *addr=(make8(id,0) >> 3) & 0x1F;
   *addr|=(make8(id,1) << 5) & 0xE0;
}

////////////////////////////////////////////////////////////////////////////////
//
// can_set_extended_id
//
// sets an extended id
//
// Parameters:
//      addr - the address that is to be set to the id
//      id - the actual id
//
////////////////////////////////////////////////////////////////////////////////

void can_set_extended_id(int8 *addr, int32 id)
{
   //eidl
   *addr=make8(id,0); //0:7

   //eidh
   addr--;
   *addr=make8(id,1); //8:15

   //sidl
   addr--;
   *addr=make8(id,2) & 0x03;   //16:17
   *addr|=(make8(id,2) << 3) & 0xE0; //18:20
   *addr|=0x08;


   //sidh
   addr--;
   *addr=((make8(id,2) >> 5) & 0x07 ); //21:23
   *addr|=((make8(id,3) << 3) & 0xF8);//24:28
}

////////////////////////////////////////////////////////////////////////
//
// can_get_id()
//
// Returns the ID of the specified buffer.  (The opposite of can_set_id())
// This is used after receiving a message, to see which ID sent the message.
//
//   Paramaters:
//     addr - pointer to first byte of ID register, starting with xxxxEIDL.
//            For example, a pointer to RXM1EIDL
//     ext - Set to TRUE if this buffer uses an extended ID, FALSE if not
//
//   Returns:
//     The ID of the buffer
//
////////////////////////////////////////////////////////////////////////
int32 can_get_id(int8 *addr, int1 ext) {
   int32 ret;
   int8 *ptr;

   ret=0;
   ptr=addr;

   if (ext) {
      ret=*ptr;  //eidl

      ptr--;     //eidh
      ret|=((int32)*ptr << 8);

      ptr--;     //sidl
      ret|=((int32)*ptr & 0x03) << 16;
      ret|=((int32)*ptr & 0xE0) << 13;

      ptr--;     //sidh
      ret|=((int32)*ptr << 21);

   }
   else {
      ptr-=2;    //sidl
      ret=((int32)*ptr & 0xE0) >> 5;

      ptr--;     //sidh
      ret|=((int32)*ptr << 3);
   }

   return(ret);
}

////////////////////////////////////////////////////////////////////////////////
//
// can_get_extended_id
//
// reads the extended identification of a buffer
//
// Parameters:
//      addr - the address that is to be read
//
////////////////////////////////////////////////////////////////////////////////
int32 can_get_extended_id(int8 *addr)
{
   int32 ret;

   ret=*addr;  //eidl

   addr--;     //eidh
   ret|=((int32)*addr << 8);

   addr--;     //sidl
   ret|=((int32)*addr & 0x03) << 16;
   ret|=((int32)*addr & 0xE0) << 13;

   addr--;     //sidh
   ret|=((int32)*addr << 21);

   return (ret);
}

////////////////////////////////////////////////////////////////////////////////
//
// can_get_standard_id
//
// reads the standard identification of a buffer
//
// Parameters
//      addr - the address that is to be read
//
////////////////////////////////////////////////////////////////////////////////
int32 can_get_standard_id(int8 *addr)
{
   int32 ret;

   addr-=2;    //sidl
   ret=((int32)*addr & 0xE0) >> 5;

   addr--;     //sidh
   ret|=((int32)*addr << 3);

   return (ret);
}

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
int8 can_putd(int32 id, int8 *data, int8 len, int8 priority, int1 ext, int1 rtr) {
   int8 i;
   int8 *txd0;
   int8 port;

   txd0=&TXRXBaD0;

    // find emtpy transmitter
    // map access bank addresses to empty transmitter
   if (!TXB0CON.txreq) 
   {
      if(curfunmode==CAN_FUN_OP_LEGACY)
         CANCON.win=CAN_WIN_TX0;
      else
         ECANCON.ewin=TX0;
      port=0;
   }
   else if (!TXB1CON.txreq)
   {
      if(curfunmode==CAN_FUN_OP_LEGACY)
         CANCON.win=CAN_WIN_TX1;
      else
         ECANCON.ewin=TX1;
      port=1;
   }
   else if (!TXB2CON.txreq) 
   {
      if(curfunmode==CAN_FUN_OP_LEGACY)
         CANCON.win=CAN_WIN_TX2;
      else
         ECANCON.ewin=TX2;
      port=2;
   }
   else if (!B0CONT.txreq && BSEL0.b0txen) 
   {
      ECANCON.ewin=TXRX0;
      port=3;
   }
   else if (!B1CONT.txreq && BSEL0.b1txen) 
   {
      ECANCON.ewin=TXRX1;
      port=4;
   }
   else if (!B2CONT.txreq && BSEL0.b2txen) 
   {
      ECANCON.ewin=TXRX2;
      port=5;
   }
   else if (!B3CONT.txreq && BSEL0.b3txen) 
   {
      ECANCON.ewin=TXRX3;
      port=6;
   }
   else if (!B4CONT.txreq && BSEL0.b4txen) 
   {
      ECANCON.ewin=TXRX4;
      port=7;
   }
   else if (!B5CONT.txreq && BSEL0.b5txen) 
   {
      ECANCON.ewin=TXRX5;
      port=8;
   }
   else 
   {
      #if CAN_DO_DEBUG
         can_debug("\r\nCAN_PUTD() FAIL: NO OPEN TX BUFFERS\r\n");
      #endif
      return(0xFF);
   }

   //set priority.
   TXBaCON.txpri=priority;

   //set tx mask
   can_set_id(TXRXBaID, id, ext);

   //set tx data count
   TXBaDLC=len;
   TXBaDLC.rtr=rtr;

    for (i=0; i<len; i++) {
      *txd0=*data;
      txd0++;
      data++;
    }

   //enable transmission
   TXBaCON.txreq=1;

   if(curfunmode==CAN_FUN_OP_LEGACY)
      CANCON.win=CAN_WIN_RX0;
   else
      ECANCON.ewin=RX0;

   #if CAN_DO_DEBUG
            can_debug("\r\nCAN_PUTD(): BUFF=%U ID=%LX LEN=%U PRI=%U EXT=%U RTR=%U\r\n", port, id, len, priority, ext, rtr);
            if ((len)&&(!rtr)) {
               data-=len;
               can_debug("  DATA = ");
               for (i=0;i<len;i++) {
                  can_debug("%X ",*data);
                  data++;
               }
               can_debug("\r\n");
            }
   #endif

   return(port);
}

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
int1 can_getd(int32 &id, int8 *data, int8 &len, struct rx_stat &stat)
{
   int8 i;
   int8 *ptr;

   if (RXB0CON.rxful)
   {
      if(curfunmode==CAN_FUN_OP_LEGACY)
         CANCON.win=CAN_WIN_RX0;
      else
         ECANCON.ewin=RX0;          // this is if in functional mode 1 or 2
         
      stat.buffer=0;

      //CAN_INT_RXB0IF=0;           // moved to end of function

      if(curfunmode)
      {
         stat.err_ovfl=COMSTAT_MODE_1.rxnovfl;
         stat.filthit=RXB0CON_MODE_1.filthit;
      }
      else
      {
         stat.err_ovfl=COMSTAT.rx0ovfl;
         COMSTAT.rx0ovfl=0;

         if (RXB0CON.rxb0dben) 
         {
            stat.filthit=RXB0CON.filthit0;
         }
      }
   }
   else if ( RXB1CON.rxful )
   {
      if(curfunmode==CAN_FUN_OP_LEGACY)
         CANCON.win=CAN_WIN_RX1;
      else
         ECANCON.ewin=RX1;
         
      stat.buffer=1;

      //CAN_INT_RXB1IF=0;           //moved to end of function

      if(curfunmode)
      {
         stat.err_ovfl=COMSTAT_MODE_1.rxnovfl;
         stat.filthit=RXB1CON_MODE_1.filthit;
      }
      else
      {
         stat.err_ovfl=COMSTAT.rx1ovfl;
         COMSTAT.rx1ovfl=0;

         stat.filthit=RXB1CON.filthit;
      }
   }
   else if (B0CONR.rxful && !BSEL0.b0txen)
   {
      ECANCON.ewin=TXRX0;
      stat.buffer=2;

      //CAN_INT_RXB1IF=0;         //moved to end of function
      
      stat.err_ovfl=COMSTAT_MODE_1.rxnovfl;
      stat.filthit=B0CONR.filhit;

     // B0CONR.rxful=0;          //moved to end of function because this shouldn't be
                                 //cleared until after data has been retrieved from buffer
   }
   else if (B1CONR.rxful && !BSEL0.b1txen)
   {
      ECANCON.ewin=TXRX1;
      stat.buffer=3;

      //CAN_INT_RXB1IF=0;         //moved to end of function

      stat.err_ovfl=COMSTAT_MODE_1.rxnovfl;
      stat.filthit=B1CONR.filhit;

      //B1CONR.rxful=0;          //moved to end of function because this shouldn't be
                                 //cleared until after data has been retrieved from buffer
   }
   else if (B2CONR.rxful && !BSEL0.b2txen)
   {
      ECANCON.ewin=TXRX2;
      stat.buffer=4;

      //CAN_INT_RXB1IF=0;         //moved to end of function

      stat.err_ovfl=COMSTAT_MODE_1.rxnovfl;
      stat.filthit=B2CONR.filhit;

      //B2CONR.rxful=0;          //moved to end of function because this shouldn't be
                                 //cleared until after data has been retrieved from buffer
   }
   else if (B3CONR.rxful && !BSEL0.b3txen)
   {
      ECANCON.ewin=TXRX3;
      stat.buffer=5;

      //CAN_INT_RXB1IF=0;         //moved to end of function

      stat.err_ovfl=COMSTAT_MODE_1.rxnovfl;
      stat.filthit=B3CONR.filhit;

      //B3CONR.rxful=0;          //moved to end of function because this shouldn't be
                                 //cleared until after data has been retrieved from buffer
   }
   else if (B4CONR.rxful && !BSEL0.b4txen)
   {
      ECANCON.ewin=TXRX4;
      stat.buffer=6;

      //CAN_INT_RXB1IF=0;         //moved to end of function

      stat.err_ovfl=COMSTAT_MODE_1.rxnovfl;
      stat.filthit=B4CONR.filhit;

      //B4CONR.rxful=0;          //moved to end of function because this shouldn't be
                                 //cleared until after data has been retrieved from buffer
   }
   else if (B5CONR.rxful && !BSEL0.b5txen)
   {
      ECANCON.ewin=TXRX5;
      stat.buffer=7;

      //CAN_INT_RXB1IF=0;         //moved to end of function

      stat.err_ovfl=COMSTAT_MODE_1.rxnovfl;
      stat.filthit=B5CONR.filhit;

      //B5CONR.rxful=0;          //moved to end of function because this shouldn't be
                                 //cleared until after data has been retrieved from buffer
   }
   else
   {
      #if CAN_DO_DEBUG
         can_debug("\r\nFAIL ON CAN_GETD(): NO MESSAGE IN BUFFER\r\n");
      #endif
      return (0);
   }

   len = RXBaDLC.dlc;
   stat.rtr=RXBaDLC.rtr;

   stat.ext=TXRXBaSIDL.ext;
   id=can_get_id(TXRXBaID,stat.ext);

   ptr = &TXRXBaD0;
   for ( i = 0; i < len; i++ ) 
   {
      *data = *ptr;
      data++;
      ptr++;
   }

   switch(stat.buffer)     //switch statement to clear rxful flag and interrupt flag
   {
      case 0:
         RXB0CON.rxful=0;
         if(curfunmode)
            CAN_INT_RXB1IF=0;
         else
            CAN_INT_RXB0IF=0;
         break;
      case 1:
         RXB1CON.rxful=0;
         CAN_INT_RXB1IF=0;
         break;
      case 2:
         B0CONR.rxful=0;
         CAN_INT_RXB1IF=0;
         break;
      case 3:
         B1CONR.rxful=0;
         CAN_INT_RXB1IF=0;
         break;
      case 4:
         B2CONR.rxful=0;
         CAN_INT_RXB1IF=0;
         break;
      case 5:
         B3CONR.rxful=0;
         CAN_INT_RXB1IF=0;
         break;
      case 6:
         B4CONR.rxful=0;
         CAN_INT_RXB1IF=0;
         break;
      case 7:
         B5CONR.rxful=0;
         CAN_INT_RXB1IF=0;
         break;
   }
   
   stat.inv=CAN_INT_IRXIF;
   CAN_INT_IRXIF = 0;
      
   // return to default addressing
   if(curfunmode==CAN_FUN_OP_LEGACY)
      CANCON.win=CAN_WIN_RX0;
   else
      ECANCON.ewin=RX0;

   #if CAN_DO_DEBUG
      can_debug("\r\nCAN_GETD(): BUFF=%U ID=%LX LEN=%U OVF=%U ", stat.buffer, id, len, stat.err_ovfl);
      can_debug("FILT=%U RTR=%U EXT=%U INV=%U", stat.filthit, stat.rtr, stat.ext, stat.inv);
      if ((len)&&(!stat.rtr)) 
      {
         data-=len;
         can_debug("\r\n    DATA = ");
         for (i=0;i<len;i++) 
         {
            can_debug("%X ",*data);
            data++;
         }
      }
      can_debug("\r\n");
   #endif

   return(1);
}

////////////////////////////////////////////////////////////////////////////////
//
// can_enable_b_transfer: mode 1 & 2
//
// Sets one of the 6 programmable buffers to be a transfer buffer.  This uses
// a simple bitwise operation so that more than one buffer can be set at a
// time.
//
// The register that contains the TX/RX information is BSEL0 and the control
//   bits take up the 6 most significant bits.  Therefore a value of 0x04 would
//   set B0 as a transmit buffer while a value of 0xFC would set all of the
// programmable buffers to transmit buffers.
//
//   Parameters:
//      b - the buffer which is to become a transfer buffer
//       enumerated as - B0,B1,B2,B3,B4,B5
//
// More information can be found on the programmable buffers in the PIC18F4580
//   datasheet, section 23.5.3
//
////////////////////////////////////////////////////////////////////////////////

void can_enable_b_transfer(PROG_BUFFER b)
{
    int8 temp;
   
   temp=BSEL0;
   temp|=b;
   
   BSEL0=temp;
}

////////////////////////////////////////////////////////////////////////////////
//
// can_enable_b_receiver: mode 1 & 2
//
// Sets one of the 6 programmable buffers to be a receiver buffer.  This uses
// a simple bitwise operation so that more than one buffer can be set at a
// time.
//
// The register that contains the TX/RX information is BSEL0 and the control
//   bits take up the 6 most significant bits.  Therefore a value of 0x04 would
//   set B0 as a receive buffer while a value of 0xFC would set all of the
// programmable buffers to receive buffers.
//
// Parameters:
//    b - the buffer which is to become a receiver buffer
//       enumerated as - B0,B1,B2,B3,B4,B5
//
// returns:
//      void
//
// More information can be found on the programmable buffers in the PIC18F4580
// datasheet, section 23.5.3
//
////////////////////////////////////////////////////////////////////////////////

void can_enable_b_receiver(PROG_BUFFER b)
{
    int8 temp;
   
   temp=BSEL0;
   temp&=~b;
   
   BSEL0=temp;
}

////////////////////////////////////////////////////////////////////////////////
//
// can_enable_rtr: mode 1 & 2
//
// Prepares one of the programmable registers to be used as an Auto-rtr buffer
//   This function allows for multiple rtr enables.
//
// Parameters:
//      b - the programmable buffer to be set to Auto-rtr mode
//       enumerated as - B0,B1,B2,B3,B4,B5
//
//      b=0x04 - B0 rtr enable
//      b=0x08 - B1 rtr enable
//      b=0x10 - B2 rtr enable
//      b=0x20 - B3 rtr enable
//    b=0x40 - B4 rtr enable
//      b=0x80 -   B5 rtr enable
//
//      any logical and (&) combination of the above will also work
//
//      b = 0x04 & 0x80 = 0x84 - B0 and B5 rtr enable
//
// returns:
//     void
//
// More information can be found on Auto-rtr in the PIC18F4580 datasheet,
//   section 23.5.4
//
////////////////////////////////////////////////////////////////////////////////

void can_enable_rtr(PROG_BUFFER b)
{

   can_enable_b_transfer ( b );

   if(bit_test(b,2))
      B0CONTA|=0x04;
   if(bit_test(b,3))
      B1CONTA|=0x04;
   if(bit_test(b,4))
      B2CONTA|=0x04;
   if(bit_test(b,5))
      B3CONTA|=0x04;
   if(bit_test(b,6))
      B4CONTA|=0x04;
   if(bit_test(b,7))
      B5CONTA|=0x04;

}

////////////////////////////////////////////////////////////////////////////////
//
// can_disable_rtr: mode 1 & 2
//
// disables certain programmable registers from being used as Auto-rtr buffers
//   This function allows for multiple rtr disables.
//
// Parameters:
//      b - the programmable buffer to be set to Auto-rtr mode
//       enumerated as B0,B1,B2,B3,B4,B5
//
//      b=0x04 - B0 rtr disable
//      b=0x08 - B1 rtr disable
//      b=0x10 - B2 rtr disable
//      b=0x20 - B3 rtr disable
//    b=0x40 - B4 rtr disable
//      b=0x80 -   B5 rtr disable
//
//      any logical or (|) combination of the above will also work
//
//      b = 0x04 | 0x80 = 0x84 - B0 and B5 rtr disable
//
// returns:
//     void
//
// More information can be found on Auto-rtr in the PIC18F4580 datasheet,
//   section 23.5.4
//
////////////////////////////////////////////////////////////////////////////////

void can_disable_rtr(PROG_BUFFER b)
{
   if(bit_test(b,2))
      B0CONTA&=0xfb;
   if(bit_test(b,3))
      B1CONTA&=0xfb;
   if(bit_test(b,4))
      B2CONTA&=0xfb;
   if(bit_test(b,5))
      B3CONTA&=0xfb;
   if(bit_test(b,6))
      B4CONTA&=0xfb;
   if(bit_test(b,7))
      B5CONTA&=0xfb;
}

////////////////////////////////////////////////////////////////////////////////
//
// can_load_rtr: mode 1 & 2
//
//   Loads a SINGLE b buffer with data to be sent on the occurence of an rtr.
//
// Parameters:
//
//      b - The programmable buffer to be filled
//       enumerated as - B0,B1,B2,B3,B4,B5
//      data - the address of the first data byte
//      len - the number of data bytes to be copied
//
// Returns:
//      void
//
//   Note, the programmer must set the appropriate rtr registers in order for the
//         rtr function to work.  This function does not set any of those
//         registers.  For example, to enable B0 as an Auto-rtr buffer, the
//         functions:
//
//         can_enable_b_transfer(B0);
//         can_enable_rtr(B0);
//         function to link acceptance filter to B0
//
//         need to be called.
//
//   can_load_rtr does no damage to the current mode of the ECAN
//
// More information on the Auto-rtr can be found in the PIC18F4580 datasheet
// section 23.5.4
//
////////////////////////////////////////////////////////////////////////////////
void can_load_rtr (PROG_BUFFER b, int8 *data, int8 len)
{

   int8 *ptr;

   //do no damage to the current mode;
   curmode = CANSTAT.opmode;

   can_set_mode(CAN_OP_CONFIG);

   if(bit_test(b,2))
   {
      ptr=&B0D0;
      B0DLCT=len;
   }
   else if (bit_test(b,3))
   {
      ptr=&B1D0;
      B1DLCT=len;
   }
   else if (bit_test(b,4))
   {
      ptr=&B2D0;
      B2DLCT=len;
   }
   else if (bit_test(b,5))
   {
      ptr=&B3D0;
      B3DLCT=len;
   }
   else if (bit_test(b,6))
   {
      ptr=&B4D0;
      B4DLCT=len;
   }
   else if (bit_test(b,7))
   {
      ptr=&B5D0;
      B5DLCT=len;
   }
   else
      return;

   for(;len>0;len--) {
      *ptr=*data;
      ptr++;
      data++;
   }

   can_set_mode(curmode);
}

////////////////////////////////////////////////////////////////////////////////
//
// can_enable_filter: mode 1 , 2 & 3
//
//   Enables a given acceptance filter
//
// Parameters:
//      filter - the filter that is to be enabled
//         enumerated as - RXF0EN-RXF15EN
//
// Returns:
//    void
//
// More information can be found on Acceptance Filters in the PIC18F4580
// datasheet, Section 23.8
//
////////////////////////////////////////////////////////////////////////////////
void can_enable_filter(int16 filter)
{
   int16 *ptr;

   curmode=CANSTAT.opmode;

   can_set_mode(CAN_OP_CONFIG);

   ptr = &RXFCON0;

   *ptr|=filter;

   can_set_mode(curmode);
}

////////////////////////////////////////////////////////////////////////////////
//
// can_disable_filter: mode 0 , 1 & 2
//
// Disables a given acceptance filter
//
// Parameters:
//      filter - the filter that is to be disabled
//        enumerated as - RXF0EN-RXF15EN
//
// Returns:
//      void;
//
// More information can be found on Acceptance Filters in the PIC18F4580
// datasheet, Section 23.8
//
////////////////////////////////////////////////////////////////////////////////
void can_disable_filter(int16 filter)
{
   int16 *ptr;

   curmode=CANSTAT.opmode;

   can_set_mode(CAN_OP_CONFIG);

   ptr = &RXFCON0;

   *ptr&=~filter;

   can_set_mode(curmode);
}

////////////////////////////////////////////////////////////////////////////////
//
// can_associate_filter_to_buffer: mode 1 & 2
//
// Associates a given buffer with a given filter register
//
// Parameters:
//      buffer - the buffer that is to be associated with the filter
//        enumerated as
//               ARXB0-ARXB1 - Dedicated recieve buffers RX0-RX1
//               AB0-AB5 - Programmable recieve buffers B0-B5
//
//      filter - the filter that is to be associated with the buffer
//         enumerated as
//               F0BP-F15BP - Filters 0 - 15
//
//
// Returns:
//      void
//
//   More information can be found on Acceptance Filters in the PIC18F4580
// datasheet, Section 23.8
//
////////////////////////////////////////////////////////////////////////////////
void can_associate_filter_to_buffer(CAN_FILTER_ASSOCIATION_BUFFERS buffer, CAN_FILTER_ASSOCIATION filter)
{
   int8 *ptr;

   curmode=CANSTAT.opmode;

   can_set_mode(CAN_OP_CONFIG);

   ptr=(filter>>1)|0x0DE0;

   if((filter & 0x01) == 1)
   {
      *ptr&=0x0f;
      *ptr|=buffer<<4;
   }
   else
   {
      *ptr&=0xf0;
      *ptr|=buffer;
   }

   can_set_mode(curmode);
}

////////////////////////////////////////////////////////////////////////////////
//
// can_associate_filter_to_mask
//
//   Associates a given filter to a given mask
//
//   Parameters:
//      mask - the mask that is to be associated with the filter
//         enumerated as
//            ACCEPTANCE_MASK_0
//            ACCEPTANCE_MASK_1
//            FILTER_15
//            NO_MASK
//
//      filter - the filter that is to be associated with the mask
//         enumerated as
//               F0BP-F15BP - Filters 0 - 15
//
//
//   Returns:
//      void
//
// More information can be foun on Acceptance Filters in the PIC18F4580
// datasheet, Section 23.8
//
////////////////////////////////////////////////////////////////////////////////
void can_associate_filter_to_mask(CAN_MASK_FILTER_ASSOCIATE mask, CAN_FILTER_ASSOCIATION filter)
{
   int8 *ptr;

   curmode = CANSTAT.opmode;

   can_set_mode(CAN_OP_CONFIG);

   ptr=(filter>>2)|0x0DF0;

   if((filter & 0x03)==0)
   {
      *ptr&=0xfc;
      *ptr|=mask;
   }
   else if((filter & 0x03)==1)
   {
      *ptr&=0xf3;
      *ptr|=mask<<2;
   }
   else if((filter & 0x03)==2)
   {
      *ptr&=0xcf;
      *ptr|=mask<<4;
   }
   else if((filter & 0x03)==3)
   {
      *ptr&=0x3f;
      *ptr|=mask<<6;
   }

   can_set_mode(curmode);
}

////////////////////////////////////////////////////////////////////////////////
//
// can_FIFO_getd
//
// Retreives data in Mode 2
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
// More information can be found on the FIFO mode in the PIC18F4580 datasheet
// section 23.7.3
//
////////////////////////////////////////////////////////////////////////////////
int1 can_fifo_getd(int32 &id, int8 *data, int8 &len, struct rx_stat &stat )
{

   int8 i;
   int8 *ptr;

   if(!COMSTAT_MODE_2.fifoempty)          // if there is no data in the buffer
      return(0);                          // return false;

   ECANCON.ewin=CANCON_MODE_2.fp | 0x10;
   stat.buffer=CANCON_MODE_2.fp;

   //CAN_INT_RXB1IF=0;                    // moved to end of function

   stat.err_ovfl=COMSTAT_MODE_2.rxnovfl;
   stat.filthit=RXB0CON_MODE_2.filthit;

   len = RXBaDLC.dlc;
   stat.rtr=RXBaDLC.rtr;

   stat.ext=TXRXBaSIDL.ext;
   id=can_get_id(TXRXBaID,stat.ext);

   ptr = &TXRXBaD0;
   for ( i = 0; i < len; i++ ) {
       *data = *ptr;
       data++;
       ptr++;
   }

   RXB0CON_MODE_2.rxful=0;
   
   CAN_INT_RXB1IF=0;

   // return to default addressing
   ECANCON.ewin=RX0;

   stat.inv=CAN_INT_IRXIF;
   CAN_INT_IRXIF = 0;

   return(1);
}

////////////////////////////////////////////////////////////////////////////////
//
// can_t0_putd - can_t2_putd
// can_b0_putd - can_b5_putd
//
// places data to be transfered in a specified buffer
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
// More information can be found on using the transfer buffers in the PICF4580
//   datasheet section 23.6
//
////////////////////////////////////////////////////////////////////////////////

// transfer buffer 0
int1 can_t0_putd(unsigned int32 id, int8 *data, int8 len, int8 pri, int1 ext, int1 rtr)
{
   int8 *ptr;

   if(TXB0CON.txreq)
      return ( FALSE );

   can_set_id( TXB0ID, id, ext );

   TXB0DLC.dlc = len;

   TXB0CON.txpri = pri;

   TXB0DLC.rtr = rtr;

   ptr = &TXB0D0;

   for(;len>0;len--)
   {
      *ptr = *data;
      ptr++;
      data++;
   }

   TXB0CON.txreq = 1;

   return ( TRUE );
}

// trasfer buffer 1
int1 can_t1_putd(int32 id, int8 *data, int8 len, int8 pri, int1 ext, int1 rtr)
{
   int8 *ptr;

   if(TXB1CON.txreq)
      return ( FALSE );

   can_set_id( TXB1ID, id, ext );

   TXB1DLC.dlc = len;

   TXB1CON.txpri = pri;

   TXB1DLC.rtr = rtr;

   ptr = &TXB1D0;

   for(;len>0;len--)
   {
      *ptr = *data;
      ptr++;
      data++;
   }

   TXB1CON.txreq = 1;

   return ( TRUE );
}

// transfer buffer 2
int1 can_t2_putd(int32 id, int8 *data, int8 len, int8 pri, int1 ext, int1 rtr)
{
   int8 *ptr;

   if(TXB2CON.txreq)
      return ( FALSE );

   can_set_id( TXB2ID, id, ext );

   TXB2DLC.dlc = len;

   TXB2CON.txpri = pri;

   TXB2DLC.rtr = rtr;

   ptr = &TXB2D0;

   for(;len>0;len--)
   {
      *ptr = *data;
      ptr++;
      data++;
   }

   TXB2CON.txreq = 1;

   return ( TRUE );
}

// programmable buffer 0
int1 can_b0_putd(int32 id, int8 *data, int8 len, int8 pri, int1 ext, int1 rtr)
{
   int8 *txd0;

   txd0=&TXRXBaD0;

   if (B0CONT.txreq || !BSEL0.b0txen)
      return ( FALSE );

   ECANCON.ewin=TXRX0;

    //set priority.
   TXBaCON.txpri=pri;

   //set tx mask
   can_set_id(TXRXBaID, id, ext);

   //set tx data count
   TXBaDLC=len;
   TXBaDLC.rtr=rtr;

    for (;len>0;len--)
    {
      *txd0=*data;
      txd0++;
      data++;
    }

   //enable transmission
   TXBaCON.txreq=1;
   
   // return to default addressing
   ECANCON.ewin=RX0;

   return ( TRUE );

}

// programmable buffer 1
int1 can_b1_putd(int32 id, int8 *data, int8 len, int8 pri, int1 ext, int1 rtr)
{
   int8 *txd0;

   txd0=&TXRXBaD0;

   if (B1CONT.txreq || !BSEL0.b1txen)
      return ( FALSE );

   ECANCON.ewin=TXRX1;

    //set priority.
   TXBaCON.txpri=pri;

   //set tx mask
   can_set_id(TXRXBaID, id, ext);

   //set tx data count
   TXBaDLC=len;
   TXBaDLC.rtr=rtr;

    for (;len>0;len--)
    {
      *txd0=*data;
      txd0++;
      data++;
    }

   //enable transmission
   TXBaCON.txreq=1;
   
   // return to default addressing
   ECANCON.ewin=RX0;

   return ( TRUE );
}

// programmable buffer 2
int1 can_b2_putd(int32 id, int8 *data, int8 len, int8 pri, int1 ext, int1 rtr)
{
   int8 *txd0;

   txd0=&TXRXBaD0;

   if (B2CONT.txreq || !BSEL0.b2txen)
      return ( FALSE );

   ECANCON.ewin=TXRX2;

    //set priority.
   TXBaCON.txpri=pri;

   //set tx mask
   can_set_id(TXRXBaID, id, ext);

   //set tx data count
   TXBaDLC=len;
   TXBaDLC.rtr=rtr;

    for (;len>0;len--)
    {
      *txd0=*data;
      txd0++;
      data++;
    }

   //enable transmission
   TXBaCON.txreq=1;
   
   // return to default addressing
   ECANCON.ewin=RX0;

   return ( TRUE );
}

// programmable buffer 3
int1 can_b3_putd(int32 id, int8 *data, int8 len, int8 pri, int1 ext, int1 rtr)
{
   int8 *txd0;

   txd0=&TXRXBaD0;

   if (B3CONT.txreq || !BSEL0.b3txen)
      return ( FALSE );

   ECANCON.ewin=TXRX3;

    //set priority.
   TXBaCON.txpri=pri;

   //set tx mask
   can_set_id(TXRXBaID, id, ext);

   //set tx data count
   TXBaDLC=len;
   TXBaDLC.rtr=rtr;

    for (;len>0;len--)
    {
      *txd0=*data;
      txd0++;
      data++;
    }

   //enable transmission
   TXBaCON.txreq=1;
   
   // return to default addressing
   ECANCON.ewin=RX0;

   return ( TRUE );

}

// programmable buffer 4
int1 can_b4_putd(int32 id, int8 *data, int8 len, int8 pri, int1 ext, int1 rtr)
{
   int8 *txd0;

   txd0=&TXRXBaD0;

   if (B4CONT.txreq || !BSEL0.b4txen)
      return ( FALSE );

   ECANCON.ewin=TXRX4;

    //set priority.
   TXBaCON.txpri=pri;

   //set tx mask
   can_set_id(TXRXBaID, id, ext);

   //set tx data count
   TXBaDLC=len;
   TXBaDLC.rtr=rtr;

    for (;len>0;len--)
    {
      *txd0=*data;
      txd0++;
      data++;
    }

   //enable transmission
   TXBaCON.txreq=1;
   
   // return to default addressing
   ECANCON.ewin=RX0;

   return ( TRUE );
}

// programmable buffer 5
int1 can_b5_putd(int32 id, int8 *data, int8 len, int8 pri, int1 ext, int1 rtr)
{
   int8 *txd0;

   txd0=&TXRXBaD0;

   if (B5CONT.txreq || !BSEL0.b5txen)
      return ( FALSE );

   ECANCON.ewin=TXRX5;

    //set priority.
   TXBaCON.txpri=pri;

   //set tx mask
   can_set_id(TXRXBaID, id, ext);

   //set tx data count
   TXBaDLC=len;
   TXBaDLC.rtr=rtr;

   for (;len>0;len--)
   {
     *txd0=*data;
     txd0++;
     data++;
   }

   txd0=&B5D0;

   //enable transmission
   TXBaCON.txreq=1;
   
   // return to default addressing
   ECANCON.ewin=RX0;

   return ( TRUE );
}

