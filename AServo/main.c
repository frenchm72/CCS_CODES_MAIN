#include "msp.h"

#define SERVO_PORT P5
#define SERVO_PIN BIT7
#define SERVO_INST 2

#define SERVO_PERIOD 20000

#define ZERO 1000
#define FULL 6000

void delay_ms(int ms);
void StsTick_Init(void);

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	StsTick_Init();

	SERVO_PORT->SEL0 |= SERVO_PIN;                  //setting up LEDs
	SERVO_PORT->SEL1 &= ~SERVO_PIN;
	SERVO_PORT->DIR |=   SERVO_PIN;

    TIMER_A2 -> CCR[0] = SERVO_PERIOD;
    TIMER_A2 -> CCR[SERVO_INST] = 0;
    TIMER_A2 -> CCTL[SERVO_INST] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A2 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_0
            | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;

    TIMER_A2 -> CCR[SERVO_INST] = ZERO;
    delay_ms(500);

    SERVO_PORT->SEL0 &= ~SERVO_PIN;                  //setting up LEDs
    SERVO_PORT->SEL1 &= ~SERVO_PIN;
	while(1){
	    delay_ms(2000);
	    SERVO_PORT->SEL0 |= SERVO_PIN;                  //setting up LEDs
	    SERVO_PORT->SEL1 &= ~SERVO_PIN;

	    TIMER_A2 -> CCR[SERVO_INST] = FULL;
	    delay_ms(500);

	    SERVO_PORT->SEL0 &= ~SERVO_PIN;                  //setting up LEDs
	    SERVO_PORT->SEL1 &= ~SERVO_PIN;
	    delay_ms(2000);
	    SERVO_PORT->SEL0 |= SERVO_PIN;                  //setting up LEDs
	            SERVO_PORT->SEL1 &= ~SERVO_PIN;

	            TIMER_A2 -> CCR[SERVO_INST] = ZERO;
	            delay_ms(500);

	            SERVO_PORT->SEL0 &= ~SERVO_PIN;                  //setting up LEDs
	            SERVO_PORT->SEL1 &= ~SERVO_PIN;
	}

}
void StsTick_Init(void)//sys tick initialization
{
    SysTick -> CTRL = 0;//disable systick during step
    SysTick -> LOAD = 0x00FFFFFF;//max reload value
    SysTick -> VAL = 0;//clears it
    SysTick -> CTRL = 0x00000005;//enables systick 3MHz no interrupts

}
void delay_ms(int ms)//delay in milliseconds using systick
{
    SysTick->LOAD =(3000*ms)-1;
    SysTick->VAL = 0;
    while((SysTick->CTRL & BIT(16))==0);
}
