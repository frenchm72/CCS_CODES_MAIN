#include "msp.h"
#include <stdio.h>

void SysTickInt(void);
void pinConfig(void);
void delay_ms(int ms);

#define BLINK_PORT  P1
#define BLINK_PIN   BIT0
#define BLINK_TIME  1000

#define BUTTON_PORT P1
#define BUTTON_PIN  BIT5
#define BUTTON_IN   PORT1_IRQHandler

uint8_t directionFlag   = 0;
uint8_t timerFlag       = 0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	__disable_irq();
	pinConfig();
	SysTickInt();
	NVIC -> ISER[1] = 0x00000008;
	__enable_irq();
	while(1){/*printf("hello\n");delay_ms(2000);*/}
}

void SysTickInt(void)
{
    SysTick -> CTRL = 0;//disable systick during step
    SysTick -> LOAD = 3000;//max reload value
    SysTick -> VAL = 0;//clears it
    SysTick -> CTRL = 0x00000007;
}
void delay_ms(int ms)//delay in milliseconds using systick
{
    SysTick->LOAD =(3000*ms)-1;
    SysTick->VAL = 0;
    while((SysTick->CTRL & BIT(16))==0);
}
void pinConfig(void)
{
    BLINK_PORT -> SEL0 &= ~BLINK_PIN;//for LED
    BLINK_PORT -> SEL1 &= ~BLINK_PIN;
    BLINK_PORT -> DIR  |=  BLINK_PIN;
    BLINK_PORT -> OUT  &= ~BLINK_PIN;

    BUTTON_PORT -> SEL0 &= ~BUTTON_PIN;//for buttoon input pin
    BUTTON_PORT -> SEL1 &= ~BUTTON_PIN;
    BUTTON_PORT -> DIR  &= ~BUTTON_PIN;
    BUTTON_PORT -> REN  |=  BUTTON_PIN;
    BUTTON_PORT -> OUT  |=  BUTTON_PIN;
    BUTTON_PORT -> IES  |=  BUTTON_PIN;
    BUTTON_PORT -> IE   |=  BUTTON_PIN;
    BUTTON_PORT -> IFG  &= ~BUTTON_PIN;
}
void SysTick_Handler(void)
{
    if(timerFlag && !(BUTTON_PORT -> IN & BUTTON_PIN))
        {
            directionFlag = !directionFlag;
            timerFlag = 0;
        }
        //BUTTON_PORT -> IFG &= ~BUTTON_PIN;
    //background thread
if(directionFlag == 0)
{
    BLINK_PORT -> OUT |= BLINK_PIN;
}
if(directionFlag == 1)
{

    BLINK_PORT -> OUT &= ~BLINK_PIN;
}
    timerFlag = 1;
}
void BUTTON_IN(void)
{

}
