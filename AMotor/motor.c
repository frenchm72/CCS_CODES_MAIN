#include "msp.h"
#include "motor.h"
#include <stdio.h>
#include <stdlib.h>


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
    while(!(SysTick->CTRL & BIT(16))==0);
}
void initTimerA(void)
{
    TIMER_A0 -> CCR[0] = TA0_PERIOD;
    TIMER_A0 -> CCR[MOTOR_INST] = 0;
    TIMER_A0 -> CCTL[MOTOR_INST] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_0
            | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;
}
void relTimer(void)
{
    TIMER_A0 -> CTL = 0x00;
}
void initMotoPWM(void)
{
    MOTOR_PORT -> SEL0 |=  MOTOR_PIN;// t0 make the pwm signal
    MOTOR_PORT -> SEL1 &= ~MOTOR_PIN;
    MOTOR_PORT -> DIR  |=  MOTOR_PIN;
}
void relMotoPWM(void)
{
    MOTOR_PORT -> SEL0 &= ~MOTOR_PIN;//to turn the signal off
    MOTOR_PORT -> SEL1 &= ~MOTOR_PIN;
}
void setMotoPWM(int speed)
{
    TIMER_A0 -> CCR[MOTOR_INST] = speed;
}
