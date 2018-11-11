/****************************************************************************
  Title:       EGR 226 Lab 3
  Filename:    Lab_4_Part_2
  Author(s):   Mathew Yerian-French & Malcolm Macdonald
  Date:        09/21/2018
  Instructor:  Professor Scott Zuidema
  Description:
  Note:
*****************************************************************************/
#include "msp.h"
#include <stdio.h>
#include <stdlib.h>

#define DELAY 1000                                   //macro for the delay cycle function, can be easily changed
#define BDELAY 5                                    //delay for debounce

enum  states                                        //this is the set up for the states used in the program
{
    LED_OFF,                                        //State for BLACK/OFF
    LED_RED,                                        //State for RED
    LED_GREEN,                                      //State for GREEN
    LED_YELLOW                                        //State for Yellow

} state;

int butPres(uint8_t b);                             //prototype for our function to see if the button has been pressed
void sys_tic();
int __delay_ms(int ms);

void main(void)
{

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer if not might restart randomly

    uint8_t a;

    sys_tic();

    P1->SEL0 &= ~(BIT5);                            //setting up button
    P1->SEL1 &= ~(BIT5);
    P1->DIR &= ~(BIT5);
    P1->REN |= BIT5;
    P1->OUT &= ~BIT5;

    P2->SEL0 &= ~(BIT0|BIT1|BIT2);                  //setting up LEDs
    P2->SEL1 &= ~(BIT0|BIT1|BIT2);
    P2->DIR |= (BIT0|BIT1|BIT2);
    P2->OUT |= (BIT0|BIT1|BIT2);

    state = LED_OFF;                                // setting initial state to LED_OFF so the LEDs are off
    while(1)                                        //loops infinitely to run the program
   {
        switch(state)                               //this is to give an action for a given state argument
        {
        case LED_OFF:                               //this if for the color black/off
            P2->OUT &= ~(BIT0|BIT1|BIT2);           //this turns pin 0, 1, and 2 off on port 2
            if(butPres(a))                          //checks if the button has been pressed through our function
                state =LED_GREEN;                     //if this is true it changes the state to GREEN
            break;                                  //breaks out of the switch statement and continues in the while
        case LED_GREEN:
            P2->OUT &= ~(BIT0|BIT1);                //this turns pin 0 and 2 off on port 2
            P2->OUT |= (BIT2);                      //this turns on P2.1 to make color GREEN
            __delay_ms(DELAY);                      //delay of 1 second to wait to change colors while being held
            if(butPres(a))                          //checks if the button has been pressed through our function
                state =LED_YELLOW;                    //if this is true it changes the state to YELLOW
            break;                                  //breaks out of the switch statement and continues in the while
        case LED_YELLOW:
            P2->OUT &= ~(BIT0|BIT2);                //this turns pin 0 and 1 off on port 2
            P2->OUT |= (BIT1);                      //this turns on P2.2 to make color YELLOW
            __delay_ms(DELAY);                      //delay of 1 second to wait to change colors while being held
            if(butPres(a))                          //checks if the button has been pressed through our function
                state =LED_RED;                     //if this is true it changes the state to RED
            break;                                  //breaks out of the switch statement and continues in the while
        case LED_RED:
            P2->OUT &= ~(BIT1|BIT2);                //this turns pin 1 and 2 off on port 2
            P2->OUT |= (BIT0);                      //this turns on P2.0 to make color RED
            __delay_ms(DELAY);                      //delay of 1 second to wait to change colors while being held
            if(butPres(a))                          //checks if the button has been pressed through our function
                state =LED_GREEN;                   //if this is true it changes the state to GREEN
            break;                                  //breaks out of the switch statement and continues in the while
        }
    }

}
int butPres(uint8_t b)                              //This is our function to see if the button has been pressed
{
    if(!((P1->IN & BIT5)))                          //This action executes if button is pressed
    {
        __delay_ms(BDELAY);                          //This is our debouncer, its immediately after  the button being pressed
      //  while(!((P1->IN & BIT5))){};               //This is for the button when it its being continually pressed
        b = 1;                                      //This is to return a value after the button is released
    }
    else                                            //This is if the button was not pressed
    {
        b = 0;                                      //makes b 0 because the button wasn't pressed
    }
    return b;                                       //Returns the value of b 1/0 pressed/not
}
void sys_tic()
{
    SysTick->CTRL = 0;                              //off
    SysTick->LOAD = 0xBB8;
    SysTick->VAL = 0;
    SysTick->CTRL = 5;                              //enables bit0and2
}
int __delay_ms(int ms)
{
    SysTick->LOAD =(3000*ms)-1;
    SysTick->VAL = 0;
    while((SysTick->CTRL & BIT(16))==0);
    return ms;
}
