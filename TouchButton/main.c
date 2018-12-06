#include "msp.h"
#include <stdio.h>
#include <stdlib.h>

#define TOUCH_PORT P1
#define TOUCH_PIN BIT5

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	TOUCH_PORT->SEL0 &= ~(TOUCH_PIN);                            //setting up button
	TOUCH_PORT->SEL1 &= ~(TOUCH_PIN);
	TOUCH_PORT->DIR |= ~(TOUCH_PIN);
	TOUCH_PORT->REN |= TOUCH_PIN;
	TOUCH_PORT->OUT |= TOUCH_PIN;
while(1)
{
	if(TOUCH_PORT->IN & TOUCH_PIN)
	    printf("Not Pressed\n");
	else
	    printf("Pressed\n");
}
}
