/****************************************************************************
  Title:       EGR 226 Lab 7
  Filename:    Lab_7_Lab_Part_
  Author(s):   Mathew Yerian-French
  Date:        10/10/2018
  Instructor:  Professor Scott Zuidema
  Description: This program uses cases to get different motor speeds using PWM
               signals inside of the cases. i have also set up some functions
               to create this PWM signal. this is located in motor.c file
               All of my macros are defined in my header file motor.h that
               makes it easier o change variables in the program i don't have
               a board so I'm not sure if this is exactly working but i know
               it should still be a solid program. i took it step by step to
               make sure it makes sense.
  Note:
*****************************************************************************/
#include "msp.h"
#include "motor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

    uint16_t speed, keyS = 0x0000;
    char new;

enum states
{
    MOTO_0,MOTO_1,MOTO_2,MOTO_3,MOTO_4,MOTO_5,MOTO_6,MOTO_7,MOTO_8,MOTO_9,WRONG
} state ;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    KeyInit();
    StsTick_Init();
    initTimerA();
    initMotoPWM();



    printf("Enter a Speed For the Motor From 1->9\n");

while(1)
{
    getKey();
    setMotoPWM(speed);

    switch(state)
    {
    case MOTO_0:
        printf("Speed 0 - NO Power!!\n");
        break;
    case MOTO_1:
        printf("Speed 1!!\n");
        break;
    case MOTO_2:
        printf("Speed 2!!\n");
        break;
    case MOTO_3:
        printf("Speed 3!!\n");
        break;
    case MOTO_4:
        printf("Speed 4!!\n");
        break;
    case MOTO_5:
        printf("Speed 5 - The Cup is Half Full!!\n");
        break;
    case MOTO_6:
        printf("Speed 6!!\n");
        break;
    case MOTO_7:
        printf("Speed 7!!\n");
        break;
    case MOTO_8:
        printf("Speed 8!!\n");
        break;
    case MOTO_9:
        printf("Speed 9 - Full Juice!!\n");
        break;
    case WRONG:
        printf("Wrong Button\n");
        getKey();
        setMotoPWM(speed);
        break;
    }
}
}


void getKey(void)
{
        keyS = readKey();//gets a value from the matrix keypad as an uint16_t
        new = numTOstring(keyS);//converts int to a string
        keyS = 0;
}
char numTOstring(uint16_t keyS)//this function converts the int to a string
{
    char myNum;

    if(keyS == 1)      { myNum = '#';   state = WRONG;  speed = 0;              }
    if(keyS == 2)      { myNum = '9';   state = MOTO_9; speed = MOTOSPD*9;        }
    if(keyS == 4)      { myNum = '6';   state = MOTO_6; speed = MOTOSPD*6;  }
    if(keyS == 8)      { myNum = '3';   state = MOTO_3; speed = MOTOSPD*3;  }

    if(keyS == 16)     { myNum = '0';   state = MOTO_0; speed = 0;              }
    if(keyS == 32)     { myNum = '8';   state = MOTO_8; speed = MOTOSPD*8;  }
    if(keyS == 64)     { myNum = '5';   state = MOTO_5; speed = MOTOSPD*5;  }
    if(keyS == 128)    { myNum = '2';   state = MOTO_2; speed = MOTOSPD*2;  }

    if(keyS == 256)    { myNum = '*';   state = WRONG;  speed = 0;              }
    if(keyS == 512)    { myNum = '7';   state = MOTO_7; speed = MOTOSPD*7;  }
    if(keyS == 1024)   { myNum = '4';   state = MOTO_4; speed = MOTOSPD*4;  }
    if(keyS == 2048)   { myNum = '1';   state = MOTO_1; speed = MOTOSPD*1;  }

    return myNum;
}
