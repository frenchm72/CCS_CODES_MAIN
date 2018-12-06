/****************************************************************************
  Title:       EGR 226 Lab 7
  Filename:    Lab_7_Lab_Part_
  Author(s):   Mathew Yerian-French
  Date:        10/10/2018
  Instructor:  Professor Scott Zuidema
  Description:
  Note:
*****************************************************************************/
#include "msp.h"
#include "motor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

    int speed;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	speed = 0;
	while(1)
	{
        setMotoPWM(speed);
	    initMotoPWM();
        relMotoPWM();
	}
}
