#include "msp.h"
#include <stdio.h>


#define ADC_CONVERSION_RATE 30000                       // Rate for ADC conversion to occur.  Sets interrupt period for SysTick.  Set to 0.1 seconds

void ADC14init(void);
void SysTickInit(void);
void TimerA0config(void);
void SpeakerConfig(void);

int temp=0, bright = 0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;         // Stop watchdog timer
  //  __disable_irq();                                    // Disables interrupts in case they are on by default.
    TimerA0config();                                    // Configure Timer A0 to make tone
    ADC14init();                                        // Start ADC to read from potentiometer
  //  SysTickInit();                                      // Init SysTick
    SpeakerConfig();                                    // Set PWM Pins to drive Speaker
  //  __enable_irq();                                     // Enables Interrupts.
    ADC14->CTL0         |=  0b1;              //Start ADC Conversion

    while(1)
        {
        //ADC14->CTL0         |=  0b1;              //Start ADC Conversion
        temp = ADC14->MEM[1];
        __delay_cycles(3000);
        bright = ADC14->MEM[0];

                                           // Loop forever.  Main code gets stuck here.  The interrupts will now take care of the actual function of the program.
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
    TIMER_A0->CCR[0]    =    0xFFFF;                    // Initialize the period of TimerA0 PWM to the maximum.  This will change at the first interrupt.  Could be set to something else.
    TIMER_A0->CCR[1]    =    0;                         // Will reset (set to 0) immediately, so it will always be off by default
    TIMER_A0->CCTL[1]   =    0b11100000;                // Set to Reset/set Compare Mode (BITs 7-5 set to 1)
    TIMER_A0->CTL       =    0b1000010100;              // Bits 9-8 = 10 to Set to SMCLK
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
    P2->SEL0            |=   BIT4;                      // Configure P2.4 to TIMER_A0 PWM Control
    P2->SEL1            &=  ~BIT4;                      // SEL = 01 sets to PWM Control
    P2->DIR             |=   BIT4;                      // Initialize speaker pin as an output
}
void ADC14init(void)
{
    //For Analog Input 8
    P4->SEL0            |=   BIT5;                      // Select ADC Operation
    P4->SEL1            |=   BIT5;                      // SEL = 11 sets to ADC operation
    P5->SEL0            |=   BIT5;                      // Select ADC Operation
    P5->SEL1            |=   BIT5;                      // SEL = 11 sets to ADC operation

    ADC14->CTL0         =    0;                         // Disable ADC for setup

    // CTL0 Configuration
    // 31-30 = 10   to divide down input clock by 32X
    // 26    = 1    to sample based on the sample timer.  This enables the use of bits 11-8 below.
    // 21-19 = 100  for SMCLK
    // 18-17 = 01   for reading multiple channels at once
    // 11-8  = 0011 for 32 clk sample and hold time
    // 7     = 1    for multiple ADC channels being read
    // 4     = 1    to turn on ADC
    ADC14->CTL0         =    0b10000100001000100000001110010000;

    ADC14->CTL1         =    BIT4|BIT5;         // Bits 5 and 4 = 11 to enable 14 bit conversion
                                                        // Bit 23 turns on Temperature Sensor
    ADC14->MCTL[0]      =    8;                         // A8 on P4.5
    ADC14->MCTL[1]      =    0 | BIT7;                         // A0 on P5.5
                                                        // BIT7 says to stop converting after this ADC.
    ADC14->IER0         =   0;            // Interrupt on for all three conversions

    ADC14->CTL0         |=   0b10;                      // Enable Conversion
    NVIC->ISER[0]       |=   1<<ADC14_IRQn;             // Turn on ADC Interrupts in NVIC.  Equivalent to "NVIC_EnableIRQ(ADC14_IRQn);"
}


