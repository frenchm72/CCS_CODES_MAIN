/****************************************************************************
  Title:       EGR 226 Lab 6
  Filename:    Lab_6_Part_III
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



void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    uint8_t i;

    StsTick_Init();//systic initialize
    initPins();//pin initiliz
    delay_ms(60);
    initLCD();
    char lab[15] = {'L', 'A', 'B','O','R', 'A', 'T', 'O','R','Y',' ', 'O', 'V', 'E', 'R',};
    char spac[9] = {' ',' ', ' ', ' ', ' ', ' ',' ',' ',' ',};
    delay_ms(100);

     comWrit(0x80);//address  05
     delay_micro(100);
     for(i=0; i<15; i++)                             //prints all elements of message
         {
             dataWrit(lab[i]);
             delay_ms(100);
         }
         while(1)
         {
          shift_off( lab, spac);                                  //function to carry the message off the the left of the screen
          shift_on(lab);                                   //function to bring the message back from the right of the screen
         }

}

