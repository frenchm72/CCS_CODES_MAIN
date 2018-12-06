#include "msp.h"
#include <stdio.h>
#include <stdlib.h>

#define BUTTON_PORT P5
#define BUTTON_PIN BIT7
#define BUTTON_INST 2


void Timer_A2_init(void);
void ButConfig(void);

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    __disable_irq();
    ButConfig();
    NVIC -> ISER[1] = 1 << TA2_N_IRQn;
    __enable_irq();
	while(1){}
}
void Timer_A2_init(void)
{
    TIMER_A2 -> CTL =
            TIMER_A_CTL_SSEL__ACLK |
            TIMER_A_CTL_ID_3 |
            TIMER_A_CTL_MC__CONTINUOUS |
            TIMER_A_CTL_CLR;

    TIMER_A2 -> CCTL[BUTTON_INST] =
            TIMER_A_CCTLN_CM__FALLING |
            TIMER_A_CCTLN_CCIS__CCIA |
            TIMER_A_CCTLN_SCS |
            TIMER_A_CCTLN_CAP |
            TIMER_A_CCTLN_CCIE;

    TIMER_A2 -> CCTL[BUTTON_INST] &= ~TIMER_A_CCTLN_CCIFG;
}
void ButConfig(void)
{
    BUTTON_PORT -> SEL0 |= BUTTON_PIN;
    BUTTON_PORT -> SEL1 &= ~BUTTON_PIN;
    BUTTON_PORT -> DIR &= ~BUTTON_PIN;
    BUTTON_PORT -> REN |= BUTTON_PIN;
    BUTTON_PORT -> OUT |= BUTTON_PIN;
    BUTTON_PORT -> IES  |=  BUTTON_PIN;
    BUTTON_PORT -> IE   |=  BUTTON_PIN;
    BUTTON_PORT -> IFG  &= ~BUTTON_PIN;
}
void TA2_N_IRQHandler(void)
{
 static uint8_t isPressed;
 st
}
void BUTTON_IN(void)
{

}
