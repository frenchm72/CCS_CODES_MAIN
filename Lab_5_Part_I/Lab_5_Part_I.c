/****************************************************************************
  Title:       EGR 226 Lab 5
  Filename:    Lab_5_Part_I
  Author(s):   Mathew Yerian-French
  Date:        09/23/2018
  Instructor:  Professor Scott Zuidema
  Description:
  Note:
*****************************************************************************/
#include "msp.h"
#include "keypad.h"
#include <stdio.h>
#include <stdlib.h>



void main(void)
{

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog time
    KeyInit();
    uint16_t key = 0x0000;

    while(1)
    {
        key = readKey();
        //printf("0x%x\n",key);
        printNum(key);

    }
}

void printNum(uint16_t key)//prints out values
{
    if(key == 1) printf("# has been pressed\n");
    if(key == 2) printf("9 has been pressed\n");
    if(key == 4) printf("6 has been pressed\n");
    if(key == 8) printf("3 has been pressed\n");

    if(key == 16) printf("0 has been pressed\n");
    if(key == 32) printf("8 has been pressed\n");
    if(key == 64) printf("5 has been pressed\n");
    if(key == 128) printf("2 has been pressed\n");

    if(key == 256) printf("* has been pressed\n");
    if(key == 512) printf("7 has been pressed\n");
    if(key == 1024) printf("4 has been pressed\n");
    if(key == 2048) printf("1 has been pressed\n");

    _delay_ms(1000);
}
