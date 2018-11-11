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

  /*  BUTTON_PORT -> SEL0 &= ~BUTTON_PIN;
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
