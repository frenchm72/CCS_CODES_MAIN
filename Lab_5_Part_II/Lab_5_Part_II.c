/****************************************************************************
  Title:       EGR 226 Lab 5
  Filename:    Lab_5_Lab_Part_II
  Author(s):   Mathew Yerian-French
  Date:        09/28/2018
  Instructor:  Professor Scott Zuidema
  Description:
  Note:
*****************************************************************************/
#include "msp.h"
#include "keypad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint16_t keyS = 0x0000;
char key[5], key1[5], new[1];
int i, a, morKey;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    KeyInit();
    i=0;

    printf("Please Enter A 4 digit pin twice followed by the '#' key\n\n");//prompts user for a pin code twice
    getKey();

    while(1)
    {
        if(morKey==1)
        {
            getKey();
        }
    if(morKey == 0)//this is to make sure you dont need more keys
    {
        if(!(strcmp(key1,key)))                 //this checks if the 2 pins you entered are the same
        {
                printf("Your Keys Matched\n");
                break;
        }
        else                                    //this is if the pins are different and make you try again
         {
             printf("!!Your Pins Are Different!!\n Try Again\n");
             restKey();
             getKey();
         }
    }
    }
}

char numTOstring(void)//this function converts the int to a string
{
    char myNum;

    if(keyS == 1)       myNum = '#';
    if(keyS == 2)       myNum = '9';
    if(keyS == 4)       myNum = '6';
    if(keyS == 8)       myNum = '3';

    if(keyS == 16)      myNum = '0';
    if(keyS == 32)      myNum = '8';
    if(keyS == 64)      myNum = '5';
    if(keyS == 128)     myNum = '2';

    if(keyS == 256)     myNum = '*';
    if(keyS == 512)     myNum = '7';
    if(keyS == 1024)    myNum = '4';
    if(keyS == 2048)    myNum = '1';

    return myNum;
}
void getKey(void)
{
    printf("\nEnter Pin ->");

    while(i != 2)
       {
        keyS = readKey();//gets a value from the matrix keypad as an uint16_t
        new[0] = numTOstring();//converts int to a string

            if((new[0]=='#'))
                {
                    keyS = 0;
                    key[4] = '\0';
                    printf("%s\n", key);//tells user what they entered
                    if(i==1)
                    {
                        i = 0;
                        morKey=0;
                        break;
                    }
                    strcpy(key1,key);//copys to the first one entered
                    if(key[0] == '\0')
                    {
                        printf("Thats not 4 try again \n");
                        i = 0;
                        morKey=1;
                        restKey();
                        break;
                    }
                    printf("\nEnter Pin Again ->");
                    morKey=0;
                    i++;
                }

            if(keyS != 0)           //this was made to shift the chars left
                {
                    key[0] = key[1];
                    key[1] = key[2];
                    key[2] = key[3];
                    key[3] = new[0];
                    morKey=0;
                }
       }

}

void restKey()
{
    for(a=0;a<5;a++)
          {
                  key[a] = '\0';
                  key1[a] = '\0';
          }
}
