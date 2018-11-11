/****************************************************************************
  Title:       EGR 226 Lab 6
  Filename:    Lab_6
  Author(s):   Mathew Yerian-French
  Date:        09/30/2018
  Instructor:  Professor Scott Zuidema
  Description:
  Note:
*****************************************************************************/

#include "msp.h"
#include "LCD.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


void initLCD(void)
{
  comWrit(0x03);//REST 3 times
  delay_ms(10);
  comWrit(0x03);
  delay_ms(10);
  comWrit(0x03);
  delay_ms(10);

  comWrit(0x02);//4bit mode
  delay_micro(100);
  comWrit(0x02);
  comWrit(0x06);//2 lin 5x7 format
  delay_micro(100);
  comWrit(0x0F);//display on cursor blinking
  delay_micro(100);
  comWrit(0x01);//clears display
  delay_micro(100);
  comWrit(0x06);//incremnt cursor
  delay_micro(100);
}
void PulsEnablePin(void)
{
    PULSE_PORT -> OUT &= ~PULSE_PIN;
    delay_micro(10);
    PULSE_PORT -> OUT |= PULSE_PIN;
    delay_micro(10);
    PULSE_PORT -> OUT &= ~PULSE_PIN;
    delay_micro(10);
}

void pushNib(uint8_t nib)
{
    DATA_PORT -> OUT &= ~0xF0; //clears P4.4 -P4.7
    DATA_PORT -> OUT |= (nib & 0x0F)<<4;
    PulsEnablePin();
}

void pushByt(uint8_t byte)
{
    uint8_t nib;

    nib = ( byte & 0xF0)>>4;
    pushNib(nib);

    nib = (byte & 0x0F);
    pushNib(nib);
    delay_micro(100);
}

void comWrit(uint8_t com)
{
    RS_PORT -> OUT &= ~RS_PIN;
    pushByt(com);
}

void dataWrit(uint8_t data)
{
    RS_PORT -> OUT |= RS_PIN;
    pushByt(data);
}


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
}




void StsTick_Init(void)//sys tick initialization
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


void shift_off(char lab[], char spac[])
{
    int j;
    comWrit(0x0C);  //turns off cursor

    for(j=0; j<15; j++)
    {
        comWrit(0x18);  //shifts addresses to the left 15 times
    delay_ms(1000);

    if (j==7)                     //writes spaces over the necessary elements
     {
        comWrit(0x80);
         delay_ms(10);
         dataWrit(spac[0]);
         delay_ms(10);
     }
    if (j==8)
     {
        comWrit(0x81);
         delay_ms(100);
         dataWrit(spac[0]);
         delay_ms(100);
     }
    if (j==9)
    {
        comWrit(0x82);
        delay_ms(10);
        dataWrit(spac[0]);
        delay_ms(10);
    }
    if (j==10)
    {
        comWrit(0x83);
        delay_ms(10);
        dataWrit(spac[0]);
        delay_ms(10);
    }
    if (j==11)
    {
        comWrit(0x84);
        delay_ms(10);
        dataWrit(spac[0]);
        delay_ms(10);
    }
    if (j==12)
    {
        comWrit(0x85);
        delay_ms(100);
        dataWrit(spac[0]);
        delay_ms(10);
    }
    if (j==13)
    {
        comWrit(0x86);
        delay_ms(10);
        dataWrit(spac[0]);
        delay_ms(10);
    }
    }
    comWrit(0x01);             //clears display
    delay_ms(100);
    comWrit(0x02);             //returns addresses back to the original positions
    delay_ms(100);
}
void shift_on(char lab[])
{
    int i;
    comWrit(0x01);             //clears DDRAM
    delay_ms(100);
    for (i=0;i<15;i++)              //shifts cursor 15 addresses to the right
    {
        comWrit(0x1C);         //shifts right
        delay_ms(10);
    }
    comWrit(0x80);             //goes to last character spot on first line
    delay_ms(100);
    for(i=0; i<15; i++)
    {
        dataWrit (lab[i]);  //prints element of message
        delay_ms(990);
        comWrit(0x18);         //shifts address to the left
        delay_ms(10);
    }
    delay_ms(990);
}

