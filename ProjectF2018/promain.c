#include "msp.h"
#include "promain.h"
#include <stdio.h>
#include <stdlib.h>

void StsTick_Init(void)                                 //sys tick initialization
{
    SysTick -> CTRL = 0;                                //disable systick during step
    SysTick -> LOAD = 0x00FFFFFF;                       //max reload value
    SysTick -> VAL = 0;                                 //clears it
    SysTick -> CTRL = 0x00000005;                       //enables systick 3MHz interrupts

    BUTTON_PORT -> SEL0 &= ~BUTTON_PIN;                 //for emergency button input pin
    BUTTON_PORT -> SEL1 &= ~BUTTON_PIN;
    BUTTON_PORT -> DIR  &= ~BUTTON_PIN;
    BUTTON_PORT -> REN  |=  BUTTON_PIN;
    BUTTON_PORT -> OUT  |=  BUTTON_PIN;
    BUTTON_PORT -> IES  |=  BUTTON_PIN;
    BUTTON_PORT -> IE   |=  BUTTON_PIN;
    BUTTON_PORT -> IFG  = 0;

}

void delay_micro(int mic)                               //function for micro seconds using systick3
{
    SysTick -> LOAD = (mic*3) - 1;                      //sets the value to micro seconds
    SysTick -> VAL = 0;                                 //clears it
    while((SysTick->CTRL & BIT(16))==0);                //waits to count down
}
void delay_ms(int ms)                                   //delay in milliseconds using systick
{
    SysTick->LOAD =(3000*ms)-1;                         //sets the systick to milliseconds
    SysTick->VAL = 0;                                   //clears the clock
    while((SysTick->CTRL & BIT(16))==0);                //waits till time is finished
}
