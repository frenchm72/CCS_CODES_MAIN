/****************************************************************************
  Title:       EGR 226 Pre-Lab 3
  Filename:    EGR_226_908_Yerian-French_Pre_Lab_3
  Author(s):   Mathew Yerian-French
  Date:        09/12/2018
  Instructor:  Professor Scott Zuidema
  Description: This one i tried to incorporate states and functions.
               Probably did a lousy job at this but i tried.
  Note:
*****************************************************************************/
#include "msp.h"
#include <stdio.h>
#include <stdlib.h>

#define DELAY 30000

enum  states
{
    BUTTON_PRES,
    BUTTON_NOT
} state;

void butPres();
void __delay_cycle(uint16_t t); //this is from class 226-20 lecture taught in CCS

void main(void)
{

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer if not might restart randomly

    //uint8_t a;

    P1->SEL0 &= ~(BIT1|BIT4);//setting the buttons to 0
    P1->SEL1 &= ~(BIT1|BIT4);
    P1->DIR |= 0xFF;

    P1->SEL0 &= ~BIT0;
    P1->SEL1 &= ~BIT0;
    P1->DIR |= BIT0;

    P1->REN |= BIT1;
    P1->OUT |= BIT1;

    P1->OUT |= BIT0;

    state = BUTTON_NOT;

    while(1)
    {
     butPres(state);
switch(state)
{
case BUTTON_NOT:
    printf("Button NOT Pressed\n");
    break;
case BUTTON_PRES:
    printf("Button Pressed\n");
    P1->OUT |= BIT0;
    break;

}
    }
}

void butPres()
{
    if(!((P1->IN & BIT1)&&(P1->IN & BIT4)))
            {
                __delay_cycle(DELAY);
                if(!((P1->IN & BIT1)&&(P1->IN & BIT4)))
                {
                state = BUTTON_PRES;
                }
            }
            else
            {
                P1->OUT &= ~BIT0;// P1->OUT |= BIT0;
                state = BUTTON_NOT;
            }
}

void __delay_cycle(uint16_t t)
{
    volatile uint16_t i;
    for(i=0;i<t;i++);
}

/*
#include "msp.h"
#include <stdio.h>
#include <stdlib.h>

#define DELAY 30000

int butPres();
void __delay_cycle(uint16_t t); //this is from class 226-20 lecture taught in CCS


void main(void)
{

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer if not might restart randomly

    uint8_t a;

    P1->SEL0 &= ~(BIT1);
    P1->DIR |= 0xFF;

    P1->SEL0 &= ~BIT0;
    P1->DIR |= BIT0;

    while(1)
    {
       a = butPres();
       if(a)
       {
           printf("Button Pressed");
       }
       else
       {
           printf("Not Pressed");
       }
}
}

int butPres()
{
    uint8_t butt;
    if((P1->IN & BIT1)&&(P1->IN & BIT4))
            {
        __delay_cycle(DELAY);
        while((P1->IN & BIT1)&&(P1->IN & BIT4))
        {
                P1->OUT &= ~BIT0;
                butt = 1;
        }
            }
            else
            {
                P1->OUT |= BIT0;
                butt = 0;
            }
    return butt;
}

void __delay_cycle(uint16_t t)
{
    volatile uint16_t i;
    for(i=0;i<t;i++);
}*/
