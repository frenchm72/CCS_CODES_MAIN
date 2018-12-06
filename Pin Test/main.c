/****************************************************************************
  Title:       EGR 226 Lab 3
  Filename:    Lab_3_Part_I
  Author(s):   Mathew Yerian-French
  Date:        09/14/2018
  Instructor:  Professor Scott Zuidema
  Description:
  Note:
*****************************************************************************/
#include "msp.h"
#include <stdio.h>
#include <stdlib.h>

#define PORT P10
//#define PIN BIT0

void main(void)
{

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer if not might restart randomly

    PORT->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);                  //setting up LEDs
    PORT->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
    PORT->DIR |= (BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
    PORT->OUT |= (BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);

    while(1)                                        //loops infinitely to run the program
   {
       // PORT -> OUT &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
        __delay_cycles(3000000);
        PORT -> OUT |= (BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
        __delay_cycles(3000000);
    }

}
