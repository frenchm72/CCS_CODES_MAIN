#include "msp.h"
#include <stdio.h>

void TimerA0config(void);
void SpeakerConfig(void);

uint8_t temp=0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;         // Stop watchdog timer
    __disable_irq();                                    // Disables interrupts in case they are on by default.
    TimerA0config();                                    // Configure Timer A0 to make tone
    SpeakerConfig();                                    // Set PWM Pins to drive Speaker
    __enable_irq();                                     // Enables Interrupts.
    while(1)                                          // Loop forever.  Main code gets stuck here.  The interrupts will now take care of the actual function of the program.
    {
        TIMER_A2->CCR[0]    =    600+0x00000860>>2;
        TIMER_A2->CCR[1]    =    TIMER_A2->CCR[0]>>1;   // Shift right by 1 is equivalent to divide by 2.  Duty cycle is 50%
    }
}

/*----------------------------------------------------------------
 * void TimerA0config(void)
 *
 * Description: Configure Timer_A0 to run upward
 * Inputs: None
 * Outputs: None
----------------------------------------------------------------*/
void TimerA0config(void)
{
    TIMER_A2->CCR[0]    =    0xFFFF;                    // Initialize the period of TimerA0 PWM to the maximum.  This will change at the first interrupt.  Could be set to something else.
    TIMER_A2->CCR[1]    =    0;                         // Will reset (set to 0) immediately, so it will always be off by default
    TIMER_A2->CCTL[1]   =    0b11100000;                // Set to Reset/set Compare Mode (BITs 7-5 set to 1)
    TIMER_A2->CTL       =    0b1000010100;              // Bits 9-8 = 10 to Set to SMCLK
                                                        // Bits 5-4 = 01 to Set to Count Up Mode
                                                        // Bit 2 to 1 to Clear and Load Settings.
}

/*----------------------------------------------------------------
 * void SpeakerConfig(void)
 *
 * Description: Set Speaker pin to PWM controlled by Timer_A0
 * Inputs: None
 * Outputs: None
----------------------------------------------------------------*/
void SpeakerConfig(void)
{
    P5->SEL0            |=   BIT6;                      // Configure P2.4 to TIMER_A0 PWM Control
    P5->SEL1            &=  ~BIT6;                      // SEL = 01 sets to PWM Control
    P5->DIR             |=   BIT6;                      // Initialize speaker pin as an output
}

