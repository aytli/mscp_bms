/////////////////////////////////////////////////////////////////////////
////                        can-PIC24.h                              ////
////                                                                 ////
//// Prototypes, definitions, defines and macros used for and with   ////
//// the CCS CAN library for PIC24HJ and dsPIC33FJ.                  ////
////                                                                 ////
//// (see can-PIC24.c)                                               ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2010 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#ifndef __CCS_CAN24HJ_LIB_DEFINES__
#define __CCS_CAN24HJ_LIB_DEFINES__

#ifndef CAN_DO_DEBUG
 #define CAN_DO_DEBUG FALSE
#endif

#ifndef CAN_BUFFER_SIZE
   #define CAN_BUFFER_SIZE 32
#endif

#ifndef CAN2_BUFFER_SIZE
   #define CAN2_BUFFER_SIZE 32
#endif

#IFNDEF CAN_USE_EXTENDED_ID
  #define CAN_USE_EXTENDED_ID         FALSE
#ENDIF

#IFNDEF CAN_BRG_SYNCH_JUMP_WIDTH
  #define CAN_BRG_SYNCH_JUMP_WIDTH  0  //synchronized jump width (def: 1 x Tq)
#ENDIF

#IFNDEF CAN2_BRG_SYNCH_JUMP_WIDTH
  #define CAN2_BRG_SYNCH_JUMP_WIDTH  0  //synchronized jump width (def: 1 x Tq)
#ENDIF

#IFNDEF CAN_BRG_PRESCALAR
  #define CAN_BRG_PRESCALAR  4  //baud rate generator prescalar (def: 4) ( Tq = (2 x (PRE + 1))/(Fosc/2) )
#ENDIF

#IFNDEF CAN2_BRG_PRESCALAR
  #define CAN2_BRG_PRESCALAR  4  //baud rate generator prescalar (def: 4) ( Tq = (2 x (PRE + 1))/(Fosc/2) )
#ENDIF

#ifndef CAN_BRG_SEG_2_PHASE_TS
 #define CAN_BRG_SEG_2_PHASE_TS   TRUE //phase segment 2 time select bit (def: freely programmable)
#endif

#ifndef CAN2_BRG_SEG_2_PHASE_TS
 #define CAN2_BRG_SEG_2_PHASE_TS   TRUE //phase segment 2 time select bit (def: freely programmable)
#endif

#ifndef CAN_BRG_SAM
 #define CAN_BRG_SAM 1 //sample of the can bus line (def: bus line is sampled 1 times prior to sample point)
#endif

#ifndef CAN2_BRG_SAM
 #define CAN2_BRG_SAM 0 //sample of the can bus line (def: bus line is sampled 1 times prior to sample point)
#endif

#ifndef CAN_BRG_PHASE_SEGMENT_1
 #define CAN_BRG_PHASE_SEGMENT_1  5 //phase segment 1 (def: 6 x Tq)
#endif

#ifndef CAN2_BRG_PHASE_SEGMENT_1
 #define CAN2_BRG_PHASE_SEGMENT_1  5 //phase segment 1 (def: 6 x Tq)
#endif

#ifndef CAN_BRG_PROPAGATION_TIME
 #define CAN_BRG_PROPAGATION_TIME 6 //propagation time select (def: 3 x Tq)
#endif

#ifndef CAN2_BRG_PROPAGATION_TIME
 #define CAN2_BRG_PROPAGATION_TIME 2 //propagation time select (def: 3 x Tq)
#endif

#ifndef CAN_BRG_WAKE_FILTER
 #define CAN_BRG_WAKE_FILTER FALSE   //selects can bus line filter for wake up bit
#endif

#ifndef CAN2_BRG_WAKE_FILTER
 #define CAN2_BRG_WAKE_FILTER FALSE   //selects can bus line filter for wake up bit
#endif

#ifndef CAN_BRG_PHASE_SEGMENT_2
 #define CAN_BRG_PHASE_SEGMENT_2 1 //phase segment 2 time select (def: 6 x Tq)
#endif

#ifndef CAN2_BRG_PHASE_SEGMENT_2
 #define CAN2_BRG_PHASE_SEGMENT_2 5 //phase segment 2 time select (def: 6 x Tq)
#endif

#ifndef CAN_ENABLE_CAN_CAPTURE
 #define CAN_ENABLE_CAN_CAPTURE 0
#endif

////////////////////////////////////////////////////////////////////////////////
//////////////////////// ECAN Control Registers ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum CAN_OP_MODE {   CAN_OP_LISTEN_ALL=7,
                     CAN_OP_CONFIG=4,
                     CAN_OP_LISTEN=3,
                     CAN_OP_LOOPBACK=2,
                     CAN_OP_DISABLE=1,
                     CAN_OP_NORMAL=0 };
                     
// ECAN Control Register 1                     
struct CiCTRL1_struct {
   int1 win;               //0      //SFR map window select bit
   int void21:2;           //1:2    
   int1 cancap;            //3      //CAN message receive timer capture event enable bit
   int1 void4;             //4
   CAN_OP_MODE opmode:3;   //5:7    //Operation mode bits
   CAN_OP_MODE reqop:3;    //8:10   //Request operation mode bits
   int1 cancks;            //11     //CAN master clock select bit
   int1 abat;              //12     //Abort all pending transmissions bit
   int1 csidl;             //13     //Stop in idle mode bit
   int void15_14:2;        //14:15
};

struct CiCTRL1_struct C1CTRL1;
#word C1CTRL1=getenv("SFR:C1CTRL1")

#if getenv("SFR:C2CTRL1")
   struct CiCTRL1_struct C2CTRL1;
   #word C2CTRL1=getenv("SFR:C2CTRL1")
#endif

// ECAN Control Register 2
struct CiCTRL2_struct {
   int dncnt:5;            //0:4    //DeviceNet filter bit number bits
   int void15_5:11;        //5:15
};

struct CiCTRL2_struct C1CTRL2;
#word C1CTRL2=getenv("SFR:C1CTRL2")

#if getenv("SFR:C2CTRL2")
   struct CiCTRL2_struct C2CTRL2;
   #word C2CTRL2=getenv("SFR:C2CTRL2")
#endif

////////////////////////////////////////////////////////////////////////////////
/////////////////////// Baud Control Registers /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// ECAN Baud Configuration Register 1
struct CiCFG1_struct {
   int brp:6;              //0:5    //Baud Rate Prescaler bits
   int sjw:2;              //6:7    //Synchronization jump width bits
   int void15_8:8;         //8:15
};

struct CiCFG1_struct C1CFG1;
#word C1CFG1=getenv("SFR:C1CFG1")

#if getenv("SFR:C2CFG1")
   struct CiCFG1_struct C2CFG1;
   #word C2CFG1=getenv("SFR:C2CFG1")
#endif

//ECAN Baud Configuration Register 2
struct CiCFG2_struct {
   int prseg:3;            //0:2    //Propagation time segment bits
   int seg1ph:3;           //3:5    //Phase segment 1 bits
   int1 sam;               //6      //Sample of the CAN bus line bit
   int1 seg2phts;          //7      //Phase segment 2 time select bit
   int seg2ph:3;           //8:10   //Phase segment 2 bits
   int void13_11:3;        //11:13
   int1 wakfil;            //14       //Select CAN bus line filter for wake-up bit
   int1 void15;            //15
};

struct CiCFG2_struct C1CFG2;
#word C1CFG2=getenv("SFR:C1CFG2")

#if getenv("SFR:C2CFG2")
   struct CiCFG2_struct C2CFG2;
   #word C2CFG2=getenv("SFR:C2CFG2")
#endif

////////////////////////////////////////////////////////////////////////////////

//ECAN Transmit/Receive Error Count Register
struct CiEC_struct {
   uint8_t rerrcnt;        //0:7    //Receive error count bits
   uint8_t terrcnt;        //8:15   //Transmit error count bits
};

struct CiEC_struct C1EC;
#word C1EC=getenv("SFR:C1EC")

#if getenv("SFR:C2EC")
   struct CiEC_struct C2EC;
   #word C2EC=getenv("SFR:C2EC")
#endif

////////////////////////////////////////////////////////////////////////////////

enum CAN_FILTER_CONTROL { FLTEN0=0x0001, FLTEN1=0x0002, FLTEN2=0x0004, FLTEN3=0x0008,
                          FLTEN4=0x0010, FLTEN5=0x0020, FLTEN6=0x0040, FLTEN7=0x0080,
                          FLTEN8=0x0100, FLTEN9=0x0200, FLTEN10=0x0400, FLTEN11=0x0800,
                          FLTEN12=0x1000, FLTEN13=0x2000, FLTEN14=0x4000, FLTEN15=0x8000};

//ECAN Acceptance Filter Enable register
#word C1FEN1=getenv("SFR:C1FEN1")

#if getenv("SFR:C2FEN1")
   #word C2FEN1=getenv("SFR:C2FEN1")
#endif

///////////////////////////////////////////////////////////////////////////////

enum CAN_FILTER_ASSOCIATION { F0BP=0x00, F1BP=0x01, F2BP=0x02, F3BP=0x03, F4BP=0x04,
                              F5BP=0x05, F6BP=0x06, F7BP=0x07, F8BP=0x08, F9BP=0x09,
                              F10BP=0x0A,F11BP=0x0B,F12BP=0x0C,F13BP=0x0D,F14BP=0x0E,
                              F15BP=0x0F};
                              
enum CAN_FILTER_ASSOCIATION_BUFFERS { ATRB0=0x00, ATRB1=0x01, ATRB2=0x02, ATRB3=0x03, ATRB4=0x04,
                                      ATRB5=0x05, ATRB6=0x06, ATRB7=0x07, ARB8=0x08, ARB9=0x09,
                                      ARB10=0x0A, ARB11=0x0B, ARB12=0x0C, ARB13=0x0D, ARB14=0x0E,
                                      AFIFO=0x0F};

//ECAN Filter 0-3 Buffer Pointer Register
struct CiBUFPNT1_struct {
   int f0bp:4;             //0:3    //RX Buffer mask for Filter 0
   int f1bp:4;             //4:7    //RX Buffer mask for Filter 1
   int f2bp:4;             //8:11   //RX Buffer mask for Filter 2
   int f3bp:4;             //12:15  //RX Buffer mask for Filter 3
};

struct CiBUFPNT1_struct C1BUFPNT1;
#word C1BUFPNT1=getenv("SFR:C1BUFPNT1")

#if getenv("SFR:C2BUFPNT1")
   struct CiBUFPNT1_struct C2BUFPNT1;
   #word C2BUFPNT1=getenv("SFR:C2BUFPNT1")
#endif

//ECAN Filter 4-7 Buffer Pointer Register
struct CiBUFPNT2_struct {
   int f4bp:4;             //0:3    //RX Buffer mask for Filter 4
   int f5bp:4;             //4:7    //RX Buffer mask for Filter 5
   int f6bp:4;             //8:11   //RX Buffer mask for Filter 6
   int f7bp:4;             //12:15  //RX Buffer mask for Filter 7
};

struct CiBUFPNT2_struct C1BUFPNT2;
#word C1BUFPNT2=getenv("SFR:C1BUFPNT2")

#if getenv("SFR:C2BUFPNT2")
   struct CiBUFPNT2_struct C2BUFPNT2;
   #word C2BUFPNT2=getenv("SFR:C2BUFPNT2")
#endif

//ECAN Filter 8-11 Buffer Pointer Register
struct CiBUFPNT3_struct {
   int f8bp:4;             //0:3    //RX Buffer mask for Filter 8
   int f9bp:4;             //4:7    //RX Buffer mask for Filter 9
   int f10bp:4;            //8:11   //RX Buffer mask for Filter 10
   int f11bp:4;            //12:15  //RX Buffer mask for Filter 11
};

struct CiBUFPNT3_struct C1BUFPNT3;
#word C1BUFPNT3=getenv("SFR:C1BUFPNT3")

#if getenv("SFR:C2BUFPNT3")
   struct CiBUFPNT3_struct C2BUFPNT3;
   #word C2BUFPNT3=getenv("SFR:C2BUFPNT3")
#endif

//ECAN Filter 12-15 Buffer Pointer Register
struct CiBUFPNT4_struct {
   int f12bp:4;            //0:3    //RX Buffer mask for Filter 12
   int f13bp:4;            //4:7    //RX Buffer mask for Filter 13
   int f14bp:4;            //8:11   //RX Buffer mask for Filter 14
   int f15bp:4;            //12:15  //RX Buffer mask for Filter 15
};

struct CiBUFPNT4_struct C1BUFPNT4;
#word C1BUFPNT4=getenv("SFR:C1BUFPNT4")

#if getenv("SFR:C2BUFPNT4")
   struct CiBUFPNT4_struct C2BUFPNT4;
   #word C2BUFPNT4=getenv("SFR:C2BUFPNT4")
#endif

///////////////////////////////////////////////////////////////////////////////

//ECAN Acceptance Filter Standard Indentifier Register
#word C1RXF0SID=getenv("SFR:C1RXF0SID")
#word C1RXF1SID=getenv("SFR:C1RXF1SID")
#word C1RXF2SID=getenv("SFR:C1RXF2SID")
#word C1RXF3SID=getenv("SFR:C1RXF3SID")
#word C1RXF4SID=getenv("SFR:C1RXF4SID")
#word C1RXF5SID=getenv("SFR:C1RXF5SID")
#word C1RXF6SID=getenv("SFR:C1RXF6SID")
#word C1RXF7SID=getenv("SFR:C1RXF7SID")
#word C1RXF8SID=getenv("SFR:C1RXF8SID")
#word C1RXF9SID=getenv("SFR:C1RXF9SID")
#word C1RXF10SID=getenv("SFR:C1RXF10SID")
#word C1RXF11SID=getenv("SFR:C1RXF11SID")
#word C1RXF12SID=getenv("SFR:C1RXF12SID")
#word C1RXF13SID=getenv("SFR:C1RXF13SID")
#word C1RXF14SID=getenv("SFR:C1RXF14SID")
#word C1RXF15SID=getenv("SFR:C1RXF15SID")

#if getenv("SFR:C2RXF0SID")
   #word C2RXF0SID=getenv("SFR:C2RXF0SID")
   #word C2RXF1SID=getenv("SFR:C2RXF1SID")
   #word C2RXF2SID=getenv("SFR:C2RXF2SID")
   #word C2RXF3SID=getenv("SFR:C2RXF3SID")
   #word C2RXF4SID=getenv("SFR:C2RXF4SID")
   #word C2RXF5SID=getenv("SFR:C2RXF5SID")
   #word C2RXF6SID=getenv("SFR:C2RXF6SID")
   #word C2RXF7SID=getenv("SFR:C2RXF7SID")
   #word C2RXF8SID=getenv("SFR:C2RXF8SID")
   #word C2RXF9SID=getenv("SFR:C2RXF9SID")
   #word C2RXF10SID=getenv("SFR:C2RXF10SID")
   #word C2RXF11SID=getenv("SFR:C2RXF11SID")
   #word C2RXF12SID=getenv("SFR:C2RXF12SID")
   #word C2RXF13SID=getenv("SFR:C2RXF13SID")
   #word C2RXF14SID=getenv("SFR:C2RXF14SID")
   #word C2RXF15SID=getenv("SFR:C2RXF15SID")
#endif

//ECAN Acceptance Filter Extended Identifier Register
#word C1RXF0=getenv("SFR:C1RXF0EID")
#word C1RXF1=getenv("SFR:C1RXF1EID")
#word C1RXF2=getenv("SFR:C1RXF2EID")
#word C1RXF3=getenv("SFR:C1RXF3EID")
#word C1RXF4=getenv("SFR:C1RXF4EID")
#word C1RXF5=getenv("SFR:C1RXF5EID")
#word C1RXF6=getenv("SFR:C1RXF6EID")
#word C1RXF7=getenv("SFR:C1RXF7EID")
#word C1RXF8=getenv("SFR:C1RXF7EID")
#word C1RXF9=getenv("SFR:C1RXF9EID")
#word C1RXF10=getenv("SFR:C1RXF10EID")
#word C1RXF11=getenv("SFR:C1RXF11EID")
#word C1RXF12=getenv("SFR:C1RXF12EID")
#word C1RXF13=getenv("SFR:C1RXF13EID")
#word C1RXF14=getenv("SFR:C1RXF14EID")
#word C1RXF15=getenv("SFR:C1RXF15EID")

#if getenv("SFR:C2RXF0EID")
   #word C2RXF0=getenv("SFR:C2RXF0EID")
   #word C2RXF1=getenv("SFR:C2RXF1EID")
   #word C2RXF2=getenv("SFR:C2RXF2EID")
   #word C2RXF3=getenv("SFR:C2RXF3EID")
   #word C2RXF4=getenv("SFR:C2RXF4EID")
   #word C2RXF5=getenv("SFR:C2RXF5EID")
   #word C2RXF6=getenv("SFR:C2RXF6EID")
   #word C2RXF7=getenv("SFR:C2RXF7EID")
   #word C2RXF8=getenv("SFR:C2RXF8EID")
   #word C2RXF9=getenv("SFR:C2RXF9EID")
   #word C2RXF10=getenv("SFR:C2RXF10EID")
   #word C2RXF11=getenv("SFR:C2RXF11EID")
   #word C2RXF12=getenv("SFR:C2RXF12EID")
   #word C2RXF13=getenv("SFR:C2RXF13EID")
   #word C2RXF14=getenv("SFR:C2RXF14EID")
   #word C2RXF15=getenv("SFR:C2RXF15EID")
#endif

////////////////////////////////////////////////////////////////////////////////

enum CAN_MASK_FILTER_ASSOCIATION { ACCEPTANCE_MASK_0=0, ACCEPTANCE_MASK_1=1,
                                   ACCEPTANCE_MASK_2=2, NO_MASK=3};

// ECAN Filter 7-0 Mask Selection Register
struct CiFMSKSEL1_struct {
   int f0msk:2;            //0:1    // Mask Source for Filter 0 bits
   int f1msk:2;            //2:3    // Mask Source for Filter 1 bits
   int f2msk:2;            //4:5    // Mask Source for Filter 2 bits
   int f3msk:2;            //6:7    // Mask Source for Filter 3 bits
   int f4msk:2;            //8:9    // Mask Source for Filter 4 bits
   int f5msk:2;            //10:11  // Mask Source for Filter 5 bits
   int f6msk:2;            //12:13  // Mask Source for Filter 6 bits
   int f7msk:2;            //14:15  // Mask Source for Filter 7 bits
};

struct CiFMSKSEL1_struct C1FMSKSEL1;
#word C1FMSKSEL1=getenv("SFR:C1FMSKSEL1")

#if getenv("SFR:C2FMSKSEL1")
   struct CiFMSKSEL1_struct C2FMSKSEL1;
   #word C2FMSKSEL1=getenv("SFR:C2FMSKSEL1")
#endif

// ECAN Filter 15-8 Mask Selection Register
struct CiFMSKSEL2_struct {
   int f8msk:2;            //0:1    // Mask Source for Filter 8 bits
   int f9msk:2;            //2:3    // Mask Source for Filter 9 bits
   int f10msk:2;           //4:5    // Mask Source for Filter 10 bits
   int f11msk:2;           //6:7    // Mask Source for Filter 11 bits
   int f12msk:2;           //8:9    // Mask Source for Filter 12 bits
   int f13msk:2;           //10:11  // Mask Source for Filter 13 bits
   int f14msk:2;           //12:13  // Mask Source for Filter 14 bits
   int f15msk:2;           //14:15  // Mask Source for Filter 15 bits
};

struct CiFMSKSEL2_struct C1FMSKSEL2;
#word C1FMSKSEL2=getenv("SFR:C1FMSKSEL2")

#if getenv("SFR:C2FMSKSEL2")
   struct CiFMSKSEL2_struct C2FMSKSEL2;
   #word C2FMSKSEL2=getenv("SFR:C2FMSKSEL2")
#endif

// ECAN Acceptance Filter Mask Standard Identifier Register
#word C1RXM0SID=getenv("SFR:C1RXM0SID")
#word C1RXM1SID=getenv("SFR:C1RXM1SID")
#word C1RXM2SID=getenv("SFR:C1RXM2SID")

#if getenv("SFR:C2RXM0SID")
   #word C2RXM0SID=getenv("SFR:C2RXM0SID")
   #word C2RXM1SID=getenv("SFR:C2RXM1SID")
   #word C2RXM2SID=getenv("SFR:C2RXM2SID")
#endif

// ECAN Acceptance Filter Mask Extended Identifier Register
#word C1RXM0=getenv("SFR:C1RXM0EID")
#word C1RXM1=getenv("SFR:C1RXM1EID")
#word C1RXM2=getenv("SFR:C1RXM1EID")

#if getenv("SFR:C2RXM0EID")
   #word C2RXM0=getenv("SFR:C2RXM0EID")
   #word C2RXM1=getenv("SFR:C2RXM1EID")
   #word C2RXM2=getenv("SFR:C2RXM2EID")
#endif

////////////////////////////////////////////////////////////////////////////////

enum BUFFER { TRB0=0, TRB1=1, TRB2=2, TRB3=3, TRB4=4, TRB5=5, TRB6=6, TRB7=7, RB8=8,
              RB9=9, RB10=10, RB11=11, RB12=12, RB13=13, RB14=14, RB15=15, RB16=16,
              RB17=17, RB18=18, RB19=19, RB20=20, RB21=21, RB22=22, RB23=23, RB24=24,
              RB25=25, RB26=26, RB27=27, RB28=28, RB29=29, RB30=30, RB31=31};

// ECAN Receive Buffer Full Register 1
struct CiRXFUL1_struct {
   int1 rxful0;            //0      //receive buffer 0 full bit
   int1 rxful1;            //1      //receive buffer 1 full bit
   int1 rxful2;            //2      //receive buffer 2 full bit
   int1 rxful3;            //3      //receive buffer 3 full bit
   int1 rxful4;            //4      //receive buffer 4 full bit
   int1 rxful5;            //5      //receive buffer 5 full bit
   int1 rxful6;            //6      //receive buffer 6 full bit
   int1 rxful7;            //7      //receive buffer 7 full bit
   int1 rxful8;            //8      //receive buffer 8 full bit
   int1 rxful9;            //9      //receive buffer 9 full bit
   int1 rxful10;           //10     //receive buffer 10 full bit
   int1 rxful11;           //11     //receive buffer 11 full bit
   int1 rxful12;           //12     //receive buffer 12 full bit
   int1 rxful13;           //13     //receive buffer 13 full bit
   int1 rxful14;           //14     //receive buffer 14 full bit
   int1 rxful15;           //15     //receive buffer 15 full bit
};

struct CiRXFUL1_struct C1RXFUL1;   
#word C1RXFUL1=getenv("SFR:C1RXFUL1")

#if getenv("SFR:C2RXFUL1")
   struct CiRXFUL1_struct C2RXFUL1;
   #word C2RXFUL1=getenv("SFR:C2RXFUL1")
#endif

// ECAN Receive Buffer Full Regiser 2
struct CiRXFUL2_struct {
   int1 rxful16;           //0      //receive buffer 16 full bit
   int1 rxful17;           //1      //receive buffer 17 full bit
   int1 rxful18;           //2      //receive buffer 18 full bit
   int1 rxful19;           //3      //receive buffer 19 full bit
   int1 rxful20;           //4      //receive buffer 20 full bit
   int1 rxful21;           //5      //receive buffer 21 full bit
   int1 rxful22;           //6      //receive buffer 22 full bit
   int1 rxful23;           //7      //receive buffer 23 full bit
   int1 rxful24;           //8      //receive buffer 24 full bit
   int1 rxful25;           //9      //receive buffer 25 full bit
   int1 rxful26;           //10     //receive buffer 26 full bit
   int1 rxful27;           //11     //receive buffer 27 full bit
   int1 rxful28;           //12     //receive buffer 28 full bit
   int1 rxful29;           //13     //receive buffer 29 full bit
   int1 rxful30;           //14     //receive buffer 30 full bit
   int1 rxful31;           //15     //receive buffer 31 full bit
};

struct CiRXFUL2_struct C1RXFUL2;
#word C1RXFUL2=getenv("SFR:C1RXFUL2")

#if getenv("SFR:C2RXFUL2")
   struct CiRXFUL2_struct C2RXFUL2;
   #word C2RXFUL2=getenv("SFR:C2RXFUL2")
#endif

// ECAN Receive Buffer Overflow Register 1
struct CiRXOVF1_struct {
   int1 rxovf0;            //0      //receive buffer 0 overflow bit
   int1 rxovf1;            //1      //receive buffer 1 overflow bit
   int1 rxovf2;            //2      //receive buffer 2 overflow bit
   int1 rxovf3;            //3      //receive buffer 3 overflow bit
   int1 rxovf4;            //4      //receive buffer 4 overflow bit
   int1 rxovf5;            //5      //receive buffer 5 overflow bit
   int1 rxovf6;            //6      //receive buffer 6 overflow bit
   int1 rxovf7;            //7      //receive buffer 7 overflow bit
   int1 rxovf8;            //8      //receive buffer 8 overflow bit
   int1 rxovf9;            //9      //receive buffer 9 overflow bit
   int1 rxovf10;           //10     //receive buffer 10 overflow bit
   int1 rxovf11;           //11     //receive buffer 11 overflow bit
   int1 rxovf12;           //12     //receive buffer 12 overflow bit
   int1 rxovf13;           //13     //receive buffer 13 overflow bit
   int1 rxovf14;           //14     //receive buffer 14 overflow bit
   int1 rxovf15;           //15     //receive buffer 15 overflow bit
};

struct CiRXOVF1_struct C1RXOVF1;
#word C1RXOVF1=getenv("SFR:C1RXOVF1")

#if getenv("SFR:C2RXOVF1")
   struct CiRXOVF1_struct C2RXOVF1;
   #word C2RXOVF1=getenv("SFR:C2RXOVF1")
#endif

// ECAN Receive Buffer Overflow Register 2
struct CiRXOVF2_struct {
   int1 rxovf16;           //0      //receive buffer 16 overflow bit
   int1 rxovf17;           //1      //receive buffer 17 overflow bit
   int1 rxovf18;           //2      //receive buffer 18 overflow bit
   int1 rxovf19;           //3      //receive buffer 19 overflow bit
   int1 rxovf20;           //4      //receive buffer 20 overflow bit
   int1 rxovf21;           //5      //receive buffer 21 overflow bit
   int1 rxovf22;           //6      //receive buffer 22 overflow bit
   int1 rxovf23;           //7      //receive buffer 23 overflow bit
   int1 rxovf24;           //8      //receive buffer 24 overflow bit
   int1 rxovf25;           //9      //receive buffer 25 overflow bit
   int1 rxovf26;           //10     //receive buffer 26 overflow bit
   int1 rxovf27;           //11     //receive buffer 27 overflow bit
   int1 rxovf28;           //12     //receive buffer 28 overflow bit
   int1 rxovf29;           //13     //receive buffer 29 overflow bit
   int1 rxovf30;           //14     //receive buffer 30 overflow bit
   int1 rxovf31;           //15     //receive buffer 31 overflow bit
};

struct CiRXOVF2_struct C1RXOVF2;
#word C1RXOVF2=getenv("SFR:C1RXOVF2")

#if getenv("SFR:C2RXOVF2")
   struct CiRXOVF2_struct C2RXOVF2;
   #word C2RXOVF2=getenv("SFR:C2RXOVF2")
#endif

////////////////////////////////////////////////////////////////////////////////

// ECAN Tx/Rx Buffer m Control Register
struct CiTRmnCON_struct {
   int txmpri:2;           //0:1    // Message Transmission Priority bits
   int1 rtrenm;            //2      // Auto-Remote Transmit Enable bit
   int1 txreqm;            //3      // Message Send Request bit
   int1 txerrm;            //4      // Error Detected During Transmission bit
   int1 txlarbm;           //5      // Message Lost Arbitration bit
   int1 txabtm;            //6      // Message Aborted bit
   int1 txenm;             //7      // Tx/Rx Buffer Selection bit
   int txnpri:2;           //8:9    // Message Trasnmission Priority bits
   int1 rtrenn;            //10     // Auto-Remote Transmit Enable bit
   int1 txreqn;            //11     // Message Send Request bit
   int1 txerrn;            //12     // Error Detected During Transmission bit
   int1 txlarbn;           //13     // Message Lost Arbitration bit
   int1 txabtn;            //14     // Message Aborted bit
   int1 txenn;             //15     // Tx/Rx Buffer Selection bit
};

struct CiTRmnCON_struct C1TR01CON;
struct CiTRmnCON_struct C1TR23CON;
struct CiTRmnCON_struct C1TR45CON;
struct CiTRmnCON_struct C1TR67CON;
#word C1TR01CON=getenv("SFR:C1TR01CON")
#word C1TR23CON=getenv("SFR:C1TR23CON")
#word C1TR45CON=getenv("SFR:C1TR45CON")
#word C1TR67CON=getenv("SFR:C1TR67CON")

#if getenv("SFR:C2TR01CON")
   struct CiTRmnCON_struct C2TR01CON;
   struct CiTRmnCON_struct C2TR23CON;
   struct CiTRmnCON_struct C2TR45CON;
   struct CiTRmnCON_struct C2TR67CON;
   #word C2TR01CON=getenv("SFR:C2TR01CON")
   #word C2TR23CON=getenv("SFR:C2TR23CON")
   #word C2TR45CON=getenv("SFR:C2TR45CON")
   #word C2TR67CON=getenv("SFR:C2TR67CON")
#endif

#word C1RXD=0x440                            //getenv("SFR:C1RXD")
#word C1TXD=0x442                            //getenv("SFR:C1TXD")

#if getenv("SFR:C2CFG1")                  //getenv("SFR:C2RXD")
   #word C2RXD=0x540                         //getenv("SFR:C2RXD")
   #word C2TXD=0x542                         //getenv("SFR:C2TXD")
#endif

////////////////////////////////////////////////////////////////////////////////

// ECAN Interrupt Code Register
struct CiVEC_struct {
   int icode:7;            //0:6    // Interrupt Flag Code bits
   int1 void7;             //7
   int filhit:5;           //8:12   // Filter Hit Number bits
   int void15_13:3;        //13:15
};

struct CiVEC_struct C1VEC;
#word C1VEC=getenv("SFR:C1VEC")

#if getenv("SFR:C2VEC")
   struct CiVEC_struct C2VEC;
   #word C2VEC=getenv("SFR:C2VEC")
#endif

// ECAN FIF0 Control Register
struct CiFCTRL_struct {
   int fsa:5;              //0:4    // FIFO Area Starts with Buffer bits
   int void12_5:8;         //5:12
   int dmabs:3;            //13:15  // DMA Buffer Size bits
};

struct CiFCTRL_struct C1FCTRL;
#word C1FCTRL=getenv("SFR:C1FCTRL")

#if getenv("SFR:C2FCTRL")
   struct CiFCTRL_struct C2FCTRL;
   #word C2FCTRL=getenv("SFR:C2FCTRL")
#endif

// ECAN FIF0 Status Register
struct CiFIFO_struct {
   int fnrb:6;             //0:5    // FIFO Next Read Buffer Pointer bits
   int void76:2;           //6:7
   int fbp:6;              //8:13   // FIFO Buffer Pointer bits
   int void15_14:2;        //14:15
};

struct CiFIFO_struct C1FIFO;
#word C1FIFO=getenv("SFR:C1FIFO")

#if getenv("SFR:C2FIFO")
   struct CiFIFO_struct C2FIFO;
   #word C2FIFO=getenv("SFR:C2FIFO")
#endif

// ECAN Interrupt Flag Register
struct CiINTF_struct {
   int1 tbif;              //0      // TX Buffer Interrupt flag bit
   int1 rbif;              //1      // rx buffer interrupt flag bit
   int1 rbobif;            //2      // rx buffer overflow interrupt flag bit
   int1 fifoif;            //3      // fifo almost full interrupt flag bit
   int1 void4;             //4
   int1 errif;             //5      // error interrupt flag bit
   int1 wakif;             //6      // bus wake-up activity interrupt flag bit
   int1 ivrif;             //7      // invalid message received interrupt flag bit
   int1 ewarn;             //8      // transmitter or receiver in error state warning bit
   int1 rxwar;             //9      // receiver in error state warning bit
   int1 txwar;             //10     // transmitter in error state warning bit
   int1 rxbp;              //11     // receiver in error state bus passive bit
   int1 txbp;              //12     // transmitter in error state bus passive bit
   int1 txbo;              //13     // transmitter in error state bus off bit
   int void15_14:2;        //14:15
};

struct CiINTF_struct C1INTF;
#word C1INTF=getenv("SFR:C1INTF")

#if getenv("SFR:C2INTF")
   struct CiINTF_struct C2INTF;
   #word C2INTF=getenv("SFR:C2INTF")
#endif

enum INTERRUPT {TB=0x01,RB=0x02,RXOV=0x04,FIFO=0x08,ERR=0x20,WAK=0x40,IVR=0x80};

// ECAN Interrupt Enable Register
typedef struct {
   int1 tbie;              //0      //tx buffer interrupt enable bit
   int1 rbie;              //1      //rx buffer interrupt enable bit
   int1 rbovie;            //2      //rx buffer overflow interrupt enable bit
   int1 fifoie;            //3      //fifo almost full interrupt enable bit
   int1 void4;             //4
   int1 errie;             //5      //error interrupt enable bit
   int1 wakie;             //6      //bus wake-up activity interrupt enable bit
   int1 ivrie;             //7      //invalid message received interrupt enable bit
   uint8_t void15_8;       //8:15
}CiINTE_struct;

CiINTE_struct C1INTE;
#word C1INTE=getenv("SFR:C1INTE")

#if getenv("SFR:C2INTE")
   CiINTE_struct C2INTE;
   #word C2INTE=getenv("SFR:C2INTE")
#endif

////////////////////////////////////////////////////////////////////////////////

enum DMA_OPERATING_MODE { CONTINUOUS_PINGPONG_DISABLED=0x00, ONESHOT_PINGPONG_DISABLED=0x01,
                          CONTINUOUS_PINGPONG_ENABLED=0x02, ONESHOT_PINGPONG_ENABLED=0x03};
                          
enum DMA_ADDRESSING_MODE { REGISTER_INDIRECT_WITH_POST=0x00, REGISTER_INDIRECT_WITHOUT_POST=0x01,
                           PERIPHERAL_INDIRECT=0x02};
                           
enum PERIPHERAL_IRQ_NUMBER { ECAN1_RECEPTION=34, ECAN2_RECEPTION=55,
                             ECAN1_TRANSMISSION=70, ECAN2_TRANSMISSION=71};

// DMA Channel x Control Register
struct DMAxCON_struct {
   DMA_OPERATING_MODE mode:2;    //0:1    // DMA channel operating mode select bits
   int void32:2;                 //2:3
   DMA_ADDRESSING_MODE amode:2;  //4:5    // DMA channel operating mode select bits
   int void10_6:5;               //6:10
   int1 nullw;                   //11     // null data peripheral write mode select bit
   int1 half;                    //12     // early block transfer complete interrupt select bit
   int1 dir;                     //13     // transfer direction bit
   int1 size;                    //14     // data transfer size bit
   int1 chen;                    //15     // channel enable bit
};

struct DMAxCON_struct DMA0CON;
struct DMAxCON_struct DMA1CON;
#word DMA0CON=getenv("SFR:DMA0CON")
#word DMA1CON=getenv("SFR:DMA1CON")

// DMA Channel x IRG Select Register
struct DMAxREQ_stuct {
   PERIPHERAL_IRQ_NUMBER irqsel:7;  //0:6    //DMA peripheral IRQ number select bits
   uint8_t void14_7;                //7:14
   int1 force;                      //15     //force DMA transfer bit
};

struct DMAxREQ_stuct DMA0REQ;
struct DMAxREQ_stuct DMA1REQ;
#word DMA0REQ=getenv("SFR:DMA0REQ")
#word DMA1REQ=getenv("SFR:DMA1REQ")

// DMA Channel x RAM Start Address Offset Register A
#word DMA0STA=getenv("SFR:DMA0STA")
#word DMA1STA=getenv("SFR:DMA1STA")

// DMA Channel x Peripheral Address Register
#word DMA0PAD=getenv("SFR:DMA0PAD")
#word DMA1PAD=getenv("SFR:DMA1PAD")

// DMA Channel x Transfer Count Register
#word DMA0CNT=getenv("SFR:DMA0CNT")
#word DMA1CNT=getenv("SFR:DMA1CNT")

#if getenv("SFR:C2CTRL1")
   struct DMAxCON_struct DMA2CON;
   struct DMAxCON_struct DMA3CON;
   #word DMA2CON=getenv("SFR:DMA2CON")
   #word DMA3CON=getenv("SFR:DMA3CON")
   
   struct DMAxREQ_stuct DMA2REQ;
   struct DMAxREQ_stuct DMA3REQ;
   #word DMA2REQ=getenv("SFR:DMA2REQ")
   #word DMA3REQ=getenv("SFR:DMA3REQ")
   
   #word DMA2STA=getenv("SFR:DMA2STA")
   #word DMA3STA=getenv("SFR:DMA3STA")
   
   #word DMA2PAD=getenv("SFR:DMA2PAD")
   #word DMA3PAD=getenv("SFR:DMA3PAD")
   
   #word DMA2CNT=getenv("SFR:DMA2CNT")
   #word DMA3CNT=getenv("SFR:DMA3CNT")
#endif

#word DSADR=getenv("SFR:DSADR")

////////////////////////////////////////////////////////////////////////////////

//value to put in mask field to accept all incoming id's
#define CAN_MASK_ACCEPT_ALL   0

//PROTOTYPES

struct rx_stat {
   uint8_t filthit;     // fillter that allowed the frame into the buffer
   uint8_t buffer;      // recieve buffer
   int1 err_ovfl;       // buffer overflow
   int1 rtr;            // rtr requested
   int1 ext;            // extended id
   int1 inv;            // invalid id?
};

void can_init(void);
void can_set_baud(void);
void can_set_mode(CAN_OP_MODE mode);
void can_set_id(uint16_t *addr, uint32_t id, int1 ext);
void can_set_buffer_id(BUFFER buffer, uint32_t id, int1 ext);
void can_set_buffer_size(uint8_t size);
uint32_t can_get_id(BUFFER buffer, int1 ext);
int1 can_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t priority, int1 ext, int1 rtr);
int1 can_getd(uint32_t &id, uint8_t *data, uint8_t &len, struct rx_stat &stat);
void can_enable_b_transfer(BUFFER b);
void can_enable_b_receiver(BUFFER b);
void can_enable_rtr(BUFFER b);
void can_disable_rtr(BUFFER b);
void can_load_rtr (BUFFER b, uint8_t *data, uint8_t len);
void can_enable_filter(CAN_FILTER_CONTROL filter);
void can_disable_filter(CAN_FILTER_CONTROL filter);
void can_associate_filter_to_buffer(CAN_FILTER_ASSOCIATION_BUFFERS buffer, CAN_FILTER_ASSOCIATION filter);
void can_associate_filter_to_mask(CAN_MASK_FILTER_ASSOCIATION mask, CAN_FILTER_ASSOCIATION filter);
int1 can_fifo_getd(uint32_t &id, uint8_t *data, uint8_t &len, struct rx_stat &stat );
void can_config_DMA(void);
void can_enable_interrupts(INTERRUPT setting);
void can_disable_interrupt(INTERRUPT setting);


#if getenv("SFR:C2CTRL1")
   void can2_init(void);
   void can2_set_baud(void);
   void can2_set_mode(CAN_OP_MODE mode);
   void can2_set_id(uint16_t *addr, uint32_t id, int1 ext);
   void can2_set_buffer_id(BUFFER buffer, uint32_t id, int1 ext);
   void can2_set_buffer_size(uint8_t size);
   uint32_t can2_get_id(BUFFER buffer, int1 ext);
   int1 can2_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t priority, int1 ext, int1 rtr);
   int1 can2_getd(uint32_t &id, uint8_t *data, uint8_t &len, struct rx_stat & stat);
   void can2_enable_b_transfer(BUFFER b);
   void can2_enable_b_receiver(BUFFER b);
   void can2_enable_rtr(BUFFER b);
   void can2_disable_rtr(BUFFER b);
   void can2_load_rtr (BUFFER b, uint8_t *data, uint8_t len);
   void can2_enable_filter(CAN_FILTER_CONTROL filter);
   void can2_disable_filter(CAN_FILTER_CONTROL filter);
   void can2_associate_filter_to_buffer(CAN_FILTER_ASSOCIATION_BUFFERS buffer, CAN_FILTER_ASSOCIATION filter);
   void can2_associate_filter_to_mask(CAN_MASK_FILTER_ASSOCIATION mask, CAN_FILTER_ASSOCIATION filter);
   int1 can2_fifo_getd(uint32_t &id, uint8_t *data, uint8_t &len, struct rx_stat &stat );
   void can2_config_DMA(void);
   void can2_enable_interrupts(INTERRUPT setting);
   void can2_disable_interrupt(INTERRUPT setting);
#endif

#endif
