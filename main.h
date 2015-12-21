#include <24HJ128GP502.h>

#FUSES WPRES32                   //Watch Dog Timer PreScalar 1:32
#FUSES WPOSTS1                   //Watch Dog Timer PostScalar 1:1
#FUSES CKSFSM                    //Clock Switching is enabled, fail Safe clock monitor is enabled

// Use internal oscillator for delay
#use delay(internal = 20000000)

// UART port (PIC24HJ128GP502)
#use rs232(baud = 57600, xmit = PIN_B8, rcv = PIN_B9)
