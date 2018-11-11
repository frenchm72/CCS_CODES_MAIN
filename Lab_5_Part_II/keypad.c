#include "msp.h"
#include "keypad.h"
#include <stdio.h>
#include <stdlib.h>

void KeyInit(void)//sets up keypad
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

       C0_BANK -> DIR &= ~C0_PORT;//sets column 1 to input
       C0_BANK -> REN |=  C0_PORT;//turns on pull up/down resistor
       C0_BANK -> OUT &= ~C0_PORT; //sets to a pull down

       C1_BANK -> DIR &= ~C1_PORT;//sets column  to input
       C1_BANK -> REN |=  C1_PORT;//turns on pull up/down resistor
       C1_BANK -> OUT &= ~C1_PORT; //sets to a pull down

       C2_BANK -> DIR &= ~C2_PORT;//sets column 3 to input
       C2_BANK -> REN |=  C2_PORT;//turns on pull up/down resistor
       C2_BANK -> OUT &= ~C2_PORT; //sets to a pull down
}

uint16_t readKey()
{

    uint16_t keys = 0x0000; //this initial sets all the bits to so nothing is pressed and if something was it'll clear it

    COL0_ON;//turns column on one at a time
    keys |= (readRow() & 0x000F) << 0;//reads in column and puts a 0 or 1 where the button was pressed from BIT0 to BITB
    COL0_OFF;//turns off one at a time to then look at next column

    COL1_ON;
    keys |= (readRow() & 0x000F) << 4;
    COL1_OFF;

    COL2_ON;
    keys |= (readRow() & 0x000F) << 8;
    COL2_OFF;

    return keys;
}

uint8_t readRow()
{
    uint8_t rows = 0x00;
    _delay_ms(15);//debounce

    rows |= (R0_BANK -> IN & R0_PORT)?1<<0:0;// this sets the row in witch the button has been pressed and sends it to the readKey pad function
    rows |= (R1_BANK -> IN & R1_PORT)?1<<1:0;
    rows |= (R2_BANK -> IN & R2_PORT)?1<<2:0;
    rows |= (R3_BANK -> IN & R3_PORT)?1<<3:0;

    while(!(R0_BANK->IN & R0_PORT)==0);//this is for the button being held
    while(!(R1_BANK->IN & R1_PORT)==0);
    while(!(R2_BANK->IN & R2_PORT)==0);
    while(!(R3_BANK->IN & R3_PORT)==0);
    return rows;
}

int _delay_ms(int ms)//delay in milliseconds using systick
{
    SysTick->LOAD =(3000*ms)-1;
    SysTick->VAL = 0;
    while(!(SysTick->CTRL & BIT(16))==0);
    return ms;
}
