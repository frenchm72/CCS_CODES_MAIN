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

uint16_t current = 0, last = 0, period = 0, firsttime = 1, newreading = 0, detect10Hz = 0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    // set up P5.6 TimerA2 for IR emitter
    P5->SEL0 |=  BIT6;
    P5->SEL1 &= ~BIT6;
    P5->DIR  |=  BIT6;

    TIMER_A2->CTL = 0b0000001011010100;  //SMCLK, UP mode, divide by 8
    TIMER_A2->CCR[0] = 37500-1;  //10 Hz at 3MHz with Divider 8
    TIMER_A2->CCR[1] = 18750-1;  //50% Duty Cycle
    TIMER_A2->CCTL[1] = 0b11100000;

    // set up P2.4 TimerA0 for receiver
    P2->SEL0 |=  BIT4;
    P2->SEL1 &= ~BIT4;
    P2->DIR  &= ~BIT4;

    TIMER_A0->CTL = 0b0000001011010100;  //SMCLK, UP mode, divide by 8
    TIMER_A0->CCR[0] = 65535;  //Set to max for maximum capability (can capture up to 0.16 seconds)
    TIMER_A0->CCTL[1] = 0b0100100100010000;

    // set up P1.0 for onboard LED
    P1->SEL0 &= ~BIT0;
    P1->SEL1 &= ~BIT0;
    P1->DIR |= BIT0;
    P1->OUT &= ~BIT0;

    NVIC_EnableIRQ(TA0_N_IRQn); // enable interrupts for TimerA0
        __enable_interrupt(); // enable interrupts

        while(1) {
            if(detect10Hz == 0){
                P1->OUT &= ~BIT0; // turn off LED is 10Hz pulse is not detected
            }
            if(detect10Hz == 1){
                            P1->OUT |= BIT0; // turn on LED if pulse is within 5% of 10Hz
                            detect10Hz = 0;
                        }
            __delay_cycles(300000);
        }
}

// interrupt handler for TimerA0
void TA0_N_IRQHandler()
{
    if(TIMER_A0->CCTL[1] & BIT0) {
        TIMER_A0->CCTL[1] &= ~BIT0;  // Clears the interrupt flag
        current = TIMER_A0->CCR[1]; // Save current value
            period = current - last; // Calculate period

    if ( ( 35635 <period ) && ( period < 39375 ) )    // within 5% of 10Hz period
        detect10Hz = 1;
    else
        detect10Hz = 0;

    last = current; // save current value as last value
    }
}
