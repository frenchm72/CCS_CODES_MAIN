/****************************************************************************
  Title:       EGR 226
  Filename:
  Author(s):   Mathew Yerian-French
  Date:        11/11/2018
  Instructor:  Professor Scott Zuidema
  Description:
  Note:
*****************************************************************************/
#include "msp.h"
#include "LCD.h"
#include "serial.h"
#include "temp.h"
#include "promain.h"
#include "speaker.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*
 * THIS INITS EVERY PIN USED IN THIS PROJECT
 */
void initPins(void)
{
    PULSE_PORT->SEL0 &= ~PULSE_PIN;                  //setting up pulse pin
    PULSE_PORT->SEL1 &= ~PULSE_PIN;
    PULSE_PORT->DIR |= PULSE_PIN;
    PULSE_PORT->OUT |= PULSE_PIN;

    RS_PORT->SEL0 &= ~RS_PIN;                  //setting up RS pin
    RS_PORT->SEL1 &= ~RS_PIN;
    RS_PORT->DIR |= RS_PIN;
    RS_PORT->OUT |= RS_PIN;

    DATA_PORT->SEL0 &= ~DATA_PIN;                  //setting up DATA pins
    DATA_PORT->SEL1 &= ~DATA_PIN;
    DATA_PORT->DIR |= DATA_PIN;
    DATA_PORT->OUT |= DATA_PIN;

    SPEAKER_PORT->SEL0 |=   SPEAKER_PIN;                      // Configure P2.4 to TIMER_A0 PWM Control
    SPEAKER_PORT->SEL1 &=  ~SPEAKER_PIN;                      // SEL = 01 sets to PWM Control
    SPEAKER_PORT->DIR |=   SPEAKER_PIN;                      // Initialize speaker pin as an output

    WAKE_PORT->SEL0 |= (WAKE_PIN); //GPIO LED
    WAKE_PORT->SEL1 &= ~(WAKE_PIN);
    WAKE_PORT->DIR  |=  (WAKE_PIN);
    WAKE_PORT->OUT |= WAKE_PIN;

    LCDBRIGHT_PORT->SEL0 |= (LCDBRIGHT_PIN); //GPIO LCD
    LCDBRIGHT_PORT->SEL1 &= ~(LCDBRIGHT_PIN);
    LCDBRIGHT_PORT->DIR  |=  (LCDBRIGHT_PIN);
    LCDBRIGHT_PORT->OUT |= LCDBRIGHT_PIN;

    BEEP_PORT->SEL0 &= ~(BEEP_PIN); //GPIO LCD
    BEEP_PORT->SEL1 &= ~(BEEP_PIN);
    BEEP_PORT->DIR  |=  (BEEP_PIN);
    BEEP_PORT->OUT |= BEEP_PIN;

    /*  BUTTON_PORT -> SEL0 &= ~BUTTON_PIN;//PIN INTERRUPT SET UP
    BUTTON_PORT -> SEL1 &= ~BUTTON_PIN;
    BUTTON_PORT -> DIR  |=  BUTTON_PIN;
    BUTTON_PORT -> REN  |=  BUTTON_PIN;
    BUTTON_PORT -> OUT  |=  BUTTON_PIN;*/
    BUTTON_PORT -> SEL0 &= ~BUTTON_PIN;
    BUTTON_PORT -> SEL1 &= ~BUTTON_PIN;
    BUTTON_PORT -> DIR  &= ~BUTTON_PIN;
    BUTTON_PORT -> REN  |=  BUTTON_PIN;
    BUTTON_PORT -> OUT  |=  BUTTON_PIN;
    BUTTON_PORT -> IES  |=  BUTTON_PIN;
    BUTTON_PORT -> IE   |=  BUTTON_PIN;
    BUTTON_PORT -> IFG  = 0;

    /*  ONOFFUP_PORT -> SEL0 &= ~ONOFFUP_PIN;
    ONOFFUP_PORT -> SEL1 &= ~ONOFFUP_PIN;
    ONOFFUP_PORT -> DIR  |=  ONOFFUP_PIN;
    ONOFFUP_PORT -> REN  |=  ONOFFUP_PIN;
    ONOFFUP_PORT -> OUT  |=  ONOFFUP_PIN;*/
    ONOFFUP_PORT -> SEL0 &= ~ONOFFUP_PIN;
    ONOFFUP_PORT -> SEL1 &= ~ONOFFUP_PIN;
    ONOFFUP_PORT -> DIR  &= ~ONOFFUP_PIN;
    ONOFFUP_PORT -> REN  |=  ONOFFUP_PIN;
    ONOFFUP_PORT -> OUT  |=  ONOFFUP_PIN;
    ONOFFUP_PORT -> IES  |=  ONOFFUP_PIN;
    ONOFFUP_PORT -> IE   |=  ONOFFUP_PIN;
    ONOFFUP_PORT -> IFG  = 0;

    /*  SDOWN_PORT -> SEL0 &= ~SDOWN_PIN;
    SDOWN_PORT -> SEL1 &= ~SDOWN_PIN;
    SDOWN_PORT -> DIR  |=  SDOWN_PIN;
    SDOWN_PORT -> REN  |=  SDOWN_PIN;
    SDOWN_PORT -> OUT  |=  SDOWN_PIN;*/
    SDOWN_PORT -> SEL0 &= ~SDOWN_PIN;
    SDOWN_PORT -> SEL1 &= ~SDOWN_PIN;
    SDOWN_PORT -> DIR  &= ~SDOWN_PIN;
    SDOWN_PORT -> REN  |=  SDOWN_PIN;
    SDOWN_PORT -> OUT  |=  SDOWN_PIN;
    SDOWN_PORT -> IES  |=  SDOWN_PIN;
    SDOWN_PORT -> IE   |=  SDOWN_PIN;
    SDOWN_PORT -> IFG  = 0;

    /*  SEC_PORT -> SEL0 &= ~SEC_PIN;//PIN INTERRUPT SET UP
    SET_PORT -> SEL1 &= ~SET_PIN;
    SET_PORT -> DIR  |=  SET_PIN;
    SET_PORT -> REN  |=  SET_PIN;
    SET_PORT -> OUT  |=  SET_PIN;*/
    SET_PORT -> SEL0 &= ~SET_PIN;
    SET_PORT -> SEL1 &= ~SET_PIN;
    SET_PORT -> DIR  &= ~SET_PIN;
    SET_PORT -> REN  |=  SET_PIN;
    SET_PORT -> OUT  |=  SET_PIN;
    SET_PORT -> IES  |=  SET_PIN;
    SET_PORT -> IE   |=  SET_PIN;
    SET_PORT -> IFG  = 0;

    /*  SETALARM_PORT -> SEL0 &= ~SETALARM_PIN;//PIN INTERRUPT SET UP
    SETALARM_PORT -> SEL1 &= ~SETALARM_PIN;
    SETALARM_PORT -> DIR  |=  SETALARM_PIN;
    SETALARM_PORT -> REN  |=  SETALARM_PIN;
    SETALARM_PORT -> OUT  |=  SETALARM_PIN;*/
    SETALARM_PORT -> SEL0 &= ~SETALARM_PIN;
    SETALARM_PORT -> SEL1 &= ~SETALARM_PIN;
    SETALARM_PORT -> DIR  &= ~SETALARM_PIN;
    SETALARM_PORT -> REN  |=  SETALARM_PIN;
    SETALARM_PORT -> OUT  |=  SETALARM_PIN;
    SETALARM_PORT -> IES  |=  SETALARM_PIN;
    SETALARM_PORT -> IE   |=  SETALARM_PIN;
    SETALARM_PORT -> IFG  = 0;

    /*  MINSEC_PORT -> SEL0 &= ~MINSEC_PIN;  //PIN INTERRUPT SET UP
    MINSEC_PORT -> SEL1 &= ~MINSEC_PIN;
    MINSEC_PORT -> DIR  |=  MINSEC_PIN;
    MINSEC_PORT -> REN  |=  MINSEC_PIN;
    MINSEC_PORT -> OUT  |=  MINSEC_PIN;*/
    MINSEC_PORT -> SEL0 &= ~MINSEC_PIN;
    MINSEC_PORT -> SEL1 &= ~MINSEC_PIN;
    MINSEC_PORT -> DIR  &= ~MINSEC_PIN;
    MINSEC_PORT -> REN  |=  MINSEC_PIN;
    MINSEC_PORT -> OUT  |=  MINSEC_PIN;
    MINSEC_PORT -> IES  |=  MINSEC_PIN;
    MINSEC_PORT -> IE   |=  MINSEC_PIN;
    MINSEC_PORT -> IFG  = 0;

    TIMER_A0 -> CCR[0] = MAXBRIGHT;//sets up timer a0
    TIMER_A0 -> CCR[WAKE_INST] = 0;
    TIMER_A0 -> CCTL[WAKE_INST] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0 -> CCR[LCDBRIGHT_INST] = 0;
    TIMER_A0 -> CCTL[LCDBRIGHT_INST] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_0
                    | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;

    TIMER_A2->CCR[0] = 600+0x00000860>>2;
    TIMER_A2->CCR[BEEP_INST] = TIMER_A2->CCR[0]>>1;
    TIMER_A2->CCTL[BEEP_INST] = 0b11100000;                // Set to Reset/set Compare Mode (BITs 7-5 set to 1)
    TIMER_A2->CTL = 0b1000010100;              // Bits 9-8 = 10 to Set to SMCLK
}


void SysTick_Init(void)//sys tick initialization
{
    SysTick -> CTRL = 0;//disable systick during step
    SysTick -> LOAD = 0x00FFFFFF;//max reload value
    SysTick -> VAL = 0;//clears it
    SysTick -> CTRL = 0x00000005;//enables systick 3MHz no interrupts

}
void delay_micro(unsigned mic)//function for micro seconds using systick3
{
    SysTick -> LOAD = (mic*3) - 1;
    SysTick -> VAL = 0;
    while((SysTick->CTRL & BIT(16))==0);
}
void delay_ms(unsigned ms)//delay in milliseconds using systick
{
    SysTick->LOAD =(3000*ms)-1;
    SysTick->VAL = 0;
    while((SysTick->CTRL & BIT(16))==0);
}
void configRTC(void)
{
    RTC_C -> CTL0 = 0xA510;//password
    RTC_C -> CTL13 = 0x0000;//no other set up
    RTC_C -> TIM0 = 00<<8 | 00;
    RTC_C -> TIM1 = 5<<8 | 1;
}
