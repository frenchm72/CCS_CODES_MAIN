/****************************************************************************
  Title:       EGR 226 Lab 6
  Filename:    Lab_6_Part_II
  Author(s):
  Date:        09/30/2018
  Instructor:  Professor Scott Zuidema
  Description:
  Note:
*****************************************************************************/

#include "msp.h"
// IR Output Code.
// Output 10Hz on P2.4
// TA0.1

//Question for later TA1.0.  How to set it up.
//Question for later Timer32, how high can you count

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    P2->SEL0 |=  BIT4;
    P2->SEL1 &= ~BIT4;
    P2->DIR  |=  BIT4;

    // 3000000 > 65535, so we need dividers
    TIMER_A0->CTL = 0b0000001011010100;  //SMCLK, UP mode, divide by 8
    TIMER_A0->CCR[0] = 37500-1;  //10 Hz at 3MHz with Divider 8

    TIMER_A0->CCR[1] = 18750-1;  //50% Duty Cycle
//    TIMER_A0->CCR[1] = TIMER_A0->CCR[0] >> 1;  // This line works too
    TIMER_A0->CCTL[1] = 0b11100000;

    while(1);

}
