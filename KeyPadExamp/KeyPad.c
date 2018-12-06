#include "msp.h"
#include "keypad.h"
#include <stdio.h>
#include <stdlib.h>


void main(void)
{

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog time
    KeyInit();
    uint16_t key = 0x0000;

       P2->SEL0 &= ~(BIT0|BIT1|BIT2);                  //setting up LEDs
       P2->SEL1 &= ~(BIT0|BIT1|BIT2);
       P2->DIR |= (BIT0|BIT1|BIT2);
       P2->OUT |= (BIT0|BIT1|BIT2);

       P2->OUT &= ~(BIT0|BIT1|BIT2);           //this turns pin 0, 1, and 2 off on port 2
	while(1)
	{

	    key = readKeyPad();

printf("%x\n",key);
_delay_ms(1000);


	}
}

void KeyInit(void)
{
       R0_BANK -> DIR &= ~R0_PORT; //set row 1 input
       R0_BANK -> REN |=  R0_PORT; //turns on pull up/down resistor
       R0_BANK -> OUT &= ~R0_PORT;//sets to pull down

       R1_BANK -> DIR &= ~R1_PORT; //set row 2 input
       R1_BANK -> REN |=  R1_PORT; //turns on pull up/down resistor
       R1_BANK -> OUT &= ~R1_PORT;//sets to pull down

       R2_BANK -> DIR &= ~R2_PORT; //set row 3 input
       R2_BANK -> REN |=  R2_PORT; //turns on pull up/down resistor
       R2_BANK -> OUT &= ~R2_PORT;//sets to pull down

       R3_BANK -> DIR &= ~R3_PORT; //set row 4 input
       R3_BANK -> REN |=  R3_PORT; //turns on pull up/down resistor
       R3_BANK -> OUT &= ~R3_PORT;//sets to pull down

       C0_BANK -> DIR &= ~C0_PORT;//sets colm 1 to input
       C0_BANK -> REN |=  C0_PORT;//turns on pull up/down resistor
       C0_BANK -> OUT &= ~C0_PORT; //sets to a pull down

       C1_BANK -> DIR &= ~C1_PORT;//sets colm  to input
       C1_BANK -> REN |=  C1_PORT;//turns on pull up/down resistor
       C1_BANK -> OUT &= ~C1_PORT; //sets to a pull down

       C2_BANK -> DIR &= ~C2_PORT;//sets colm 3 to input
       C2_BANK -> REN |=  C2_PORT;//turns on pull up/down resistor
       C2_BANK -> OUT &= ~C2_PORT; //sets to a pull down
}

uint16_t readKeyPad()
{
    /**************************************
     KEYPAD pin READout in Return varible 'Key'
     BIT0   BIT4    BIT8
     BIT1   BIT5    BIT9
     BIT2   BIT6    BITA
     BIT3   BIT7    BITB
     **************************************/

    uint16_t keys = 0x0000;

    COL0_ON;
    keys |= (readColm() & 0x000F) << 0;
    COL0_OFF;

    COL1_ON;
    keys |= (readColm() & 0x000F) << 4;
    COL1_OFF;

    COL2_ON;
    keys |= (readColm() & 0x000F) << 8;
    COL2_OFF;
    return keys;
}

uint8_t readColm()
{
    volatile uint32_t t;
    uint8_t rows = 0x00;
    _delay_ms(5);

    rows |= (R0_BANK -> IN & R0_PORT)?1<<0:0;
    rows |= (R1_BANK -> IN & R1_PORT)?1<<1:0;
    rows |= (R2_BANK -> IN & R2_PORT)?1<<2:0;
    rows |= (R3_BANK -> IN & R3_PORT)?1<<3:0;

    return rows;
}

int _delay_ms(int ms)
{
    SysTick->LOAD =(3000*ms)-1;
    SysTick->VAL = 0;
    while(!(SysTick->CTRL & BIT(16))==0);
    return ms;
}

