#include "msp.h"
#include "serial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char readA[10] = {'R','E','A','D','A','L','A','R','M','\0'},
     readT[9] = {'R','E','A','D','T','I','M','E','\0'},
     terminal[100];//to print invalid or valid
int validFlag = 0 , i;//to set a flag if its invalid or valid

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    char string[BUFFER_SIZE]; // Creates local char array to store incoming serial commands

	__disable_irq();
    setupSerial();//sets up serial communication at 9600 8-Bit no parity

    INPUT_BUFFER[0]= '\0';  // Sets the global buffer to initial condition of empty

    __enable_irq();  // Enable all interrupts (serial)
    while(1) {
            readInput(string); // Read the input
            checkStr(string);   //checks if the string is valid
            validFlag = 0;
       }//while
}
void checkStr(char *string)
{
    if(string[0] != '\0')//if the first char is a null it wont do anything
           {
               if((strlen(string)==16)&&(string[0] == 'S')&&(string[1] == 'E')&&(string[2] == 'T')&&(string[3] == 'T')&&(string[4] == 'I')&&(string[5] == 'M')&&(string[6] == 'E')&&(string[7] == ' ')&&(string[10] == ':')&&(string[13] == ':'))//if it is 16 chars long for SETTIME HH:MM:SS
               {
                   if( ((string[8] >= '0')&&(string[8] <= '9')) && ((string[9] >= '0')&&(string[9] <= '9')) && ((string[11] >= '0')&&(string[11] <= '9')) && ((string[12] >= '0')&&(string[12] <= '9')) && ((string[14] >= '0')&&(string[14] <= '9')) && ((string[15] >= '0')&&(string[15] <= '9')))
                   {
                   sprintf(terminal, "Valid %s\n", string);
                   writeOutput(terminal);
                   validFlag = 1;
                   }
               }
               if((strlen(string)==14)&&(string[0] == 'S')&&(string[1] == 'E')&&(string[2] == 'T')&&(string[3] == 'A')&&(string[4] == 'L')&&(string[5] == 'A')&&(string[6] == 'R')&&(string[7] == 'M')&&(string[8] == ' ')&&(string[11] == ':'))//if it 14 Chars Long for SETALARM HH:MM
               {
                   //validFlag = 0;
                   if( ((string[9] >= '0')&&(string[9] <= '9')) && ((string[10] >= '0')&&(string[10] <= '9')) && ((string[12] >= '0')&&(string[12] <= '9')) && ((string[13] >= '0')&&(string[13] <= '9')))
                   {
                   sprintf(terminal, "Valid %s\n", string);
                   writeOutput(terminal);
                   validFlag = 1;
                   }
               }
               if((strlen(string)==8)&&!(strcmp(string,readT)))//if it 8 Chars Long for READTIME
               {
                   sprintf(terminal, "%s--> 12:12:56\n", string);
                   writeOutput(terminal);
                   validFlag = 1;
               }
               if((strlen(string)==9)&&!(strcmp(string,readA)))//if it 9 Chars Long for READALARM
               {
                   sprintf(terminal, "%s--> 01:22\n", string);
                   writeOutput(terminal);
                   validFlag = 1;
               }
               else if(validFlag == 0)//anything other than the rest
                              {
                               sprintf(terminal, "Invalid %s\n", string);
                               writeOutput(terminal);
                               validFlag = 0;
                               }
           }//null if
}//function

