/****************************************************************************
  Title:       EGR 226 Lab 6
  Filename:    Lab_6_Part_II
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
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	uint8_t i;

	StsTick_Init();//systic initialize
	initPins();//pin initiliz
	delay_ms(60);
	initLCD();

	char nam[7] = {'M', 'A', 'T', 'H', 'E', 'W',};
   // char lasnam[6] = {'F','R', 'E', 'N', 'C', 'H',};
    //char class[3] = {'E','G','R',};
    //char sec[3] = {'2','2','6',};
    delay_ms(100);

    comWrit(0x85);//address  05
     delay_micro(100);

    for(i=0;i<6;i++)
    {
        dataWrit(nam[i]);
        delay_ms(100);
    }
   /* comWrit(0xC5);//address  45
     delay_micro(100);
    for(i=0;i<6;i++)
    {
        dataWrit(lasnam[i]);
        delay_ms(100);
    }
    comWrit(0x96);//address  16
     delay_micro(100);
    for(i=0;i<3;i++)
    {
        dataWrit(class[i]);
        delay_ms(100);
    }
    comWrit(0xD6);//address  56
     delay_micro(100);
    for(i=0;i<3;i++)
    {
        dataWrit(sec[i]);
        delay_ms(100);
    }*/
    comWrit(0x0C);
    delay_micro(100);


}

