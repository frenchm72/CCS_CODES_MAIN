/****************************************************************************
  Title:       EGR 226 Lab 7
  Filename:    Lab_7_Lab_Part_
  Author(s):   Mathew Yerian-French
  Date:        10/10/2018
  Instructor:  Professor Scott Zuidema
  Description: Most of this code was written by multiple Professors and this
               was given in lecture 226-20 Prof. Brakora. How ever some was
               re commented in my own words to understand
  Note:
*****************************************************************************/
#include "msp.h"
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 100
char INPUT_BUFFER[BUFFER_SIZE];
uint8_t storage_location = 0; //store location
uint8_t read_location = 0; // read in varible

void writeOutput(char *string); // write output characters to the serial port
void readInput(char* string); // read input characters from INPUT_BUFFER that are valid
void setupP2(); // Sets up P1.0 as an output to drive the on board LED
void setupSerial(); // Sets up serial for use and enables interrupts

void convertStr(char *string);//converts string for duty cycle to an int
void checkStr(char *string);//checks if the string is valid
void setRPWM(void);//sets red pwm
void setGPWM(void);//sets green pwm
void setBPWM(void);//sets blue pwm

char terminal[100];//to print invalid or valid
int validFlag = 0;//to set a flag if its invalid or valid
float duty = 0.0;//duty cycle variable


int main(void)
{
    char string[BUFFER_SIZE]; // Creates local char array to store incoming serial commands
    WDT_A->CTL = WDT_A_CTL_PW |  WDT_A_CTL_HOLD; // Stop watchdog timer

    setupP2();//sets up pins for LEDs as well as initializes the timer a for the pwm
    setupSerial();//sets up serial communication at 9600 8-Bit no parity

    INPUT_BUFFER[0]= '\0';  // Sets the global buffer to initial condition of empty

    __enable_irq();  // Enable all interrupts (serial)

    while(1)
    {
        readInput(string); // Read the input
        checkStr(string);   //checks if the string is valid
        if(validFlag ==1)//if it is valid
        {
            convertStr(string);//converts the string to a duty cycle bc it was valid
            if((string[0] == 'R'))//checks if it was r for red
            {
                setRPWM();//sets the pwm
                validFlag = 0;//resets the valid flag
                //duty = 0;
            }
            else if((string[0] == 'G'))//if the string has a g for green
            {
                setGPWM();//sets pwm
                validFlag = 0;//resets flag
                // duty = 0;
            }
            else if((string[0] == 'B'))//see if the string was b for blue
            {
                setBPWM();//sets pwm
                validFlag = 0;//resets flag
                //duty = 0;
            }
        }//valid if
   }//while
}//main

void convertStr(char *string)//to converts the string
{
    duty = (((string[1] - 48)*100) + ((string[2] - 48)*10) +((string[3] - 48)));//gets the percent
    duty = duty/100.00;//converts to a decimal
    duty = 7499.00 * duty;//multiplied by full power
}

void setRPWM(void)
{
    TIMER_A0 -> CCR[1] = duty;//sets duty cycle when called
}

void setGPWM(void)
{
    TIMER_A0 -> CCR[2] = duty;//sets duty cycle when called
}

void setBPWM(void)
{
    TIMER_A0 -> CCR[3] = duty;//sets duty cycle when called
}

void checkStr(char *string)
{
    if(string[0] != '\0')//if the first char is a null it wont do anything
    {
        if(strlen(string)==4 && (string[0] == 'R' || string[0] == 'B' || string[0] == 'G'))//if it has the rgb colors and has 4 elements
        {
            if((string[1] != '0' && string[1] != '1'))//if the first number is a 1 or 0
            {
                /*  sprintf(terminal, "Invalid %s\n", string);
                    writeOutput(terminal);*/
                    validFlag = 0;
                               }
                else if((string[1]=='1' && string[2] == '0' && string[3] == '0')) // if it is 100
                {
                    /*sprintf(terminal, "Valid %s\n", string);
                      writeOutput(terminal);*/
                      validFlag = 1;
                }
                else if((string[1]=='1' && (string[2] != '0' || string[3] != '0'))) //if its 100 or more
                {
                    /* sprintf(terminal, "Invalid %s\n", string);
                       writeOutput(terminal);*/
                       validFlag = 0;
                }
                else if((string[1]=='0' && string[2] >= '0' && string[3] >= '0')) //if its any number less than 100
                {
                    /* sprintf(terminal, "Valid %s\n", string);
                                   writeOutput(terminal);*/
                    validFlag = 1;
                }
                   }
        else//anything other than the rest
        {
            /* sprintf(terminal, "Invalid %s\n", string);
                writeOutput(terminal);*/
                    validFlag = 0;
        }
    }//null if
}//function

void writeOutput(char *string)
{
    int i = 0;  // Location in the char array "string" that is being written to

    while(string[i] != '\0') {
        EUSCI_A0->TXBUF = string[i];
        i++;
        while(!(EUSCI_A0->IFG & BIT1));
    }
}

void readInput(char *string)
{
    int i = 0;  // Location in the char array "string" that is being written to
    do
    {
        // If a new line hasn't been found yet, but we are caught up to what has been received, wait here for new data
        while(read_location == storage_location && INPUT_BUFFER[read_location] != '\n');
        string[i] = INPUT_BUFFER[read_location];  // Manual copy of valid character into "string"
        INPUT_BUFFER[read_location] = '\0';
        i++; // Increment the location in "string" for next piece of data
        read_location++; // Increment location in INPUT_BUFFER that has been read
        if(read_location == BUFFER_SIZE)  // If the end of INPUT_BUFFER has been reached, loop back to 0
            read_location = 0;
    }
    while(string[i-1] != '\n'); // If a \n was just read, break out of the while loop

    string[i-1] = '\0'; // Replace the \n with a \0 to end the string when returning this function
}

void EUSCIA0_IRQHandler(void)
{
    if (EUSCI_A0->IFG & BIT0)  // Interrupt on the receive line
    {
        INPUT_BUFFER[storage_location] = EUSCI_A0->RXBUF; // store the new piece of data at the present location in the buffer
        EUSCI_A0->IFG &= ~BIT0; // Clear the interrupt flag right away in case new data is ready
        storage_location++; // update to the next position in the buffer
        if(storage_location == BUFFER_SIZE) // if the end of the buffer was reached, loop back to the start
            storage_location = 0;
    }
}

void setupP2()
{
    P2->SEL0 |= (BIT4|BIT5|BIT6); //GPIO LED RGB on board
    P2->SEL1 &= ~(BIT4|BIT5|BIT6);
    P2->DIR  |=  (BIT4|BIT5|BIT6);


    TIMER_A0 -> CCR[0] = 7499;//sets up timer a0
        TIMER_A0 -> CCR[1] = 0;
        TIMER_A0 -> CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
        TIMER_A0 -> CCR[2] = 0;
        TIMER_A0 -> CCTL[2] = TIMER_A_CCTLN_OUTMOD_7;
        TIMER_A0 -> CCR[3] = 0;
        TIMER_A0 -> CCTL[3] = TIMER_A_CCTLN_OUTMOD_7;//mode 7
        TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_0
                | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;
}

void setupSerial()
{
    P1->SEL0 |=  (BIT2 | BIT3); // P1.2 and P1.3 are EUSCI_A0 RX
    P1->SEL1 &= ~(BIT2 | BIT3); // and TX respectively.

    EUSCI_A0->CTLW0  = BIT0; // Disables EUSCI. Default configuration is 8N1
    EUSCI_A0->CTLW0 |= BIT7; // Connects to SMCLK BIT[7:6] = 10
    EUSCI_A0->CTLW0 |= 0x0001;  //BIT15 = no Parity, BIT11 = One Stop Bit
    // Baud Rate Configuration
    // 3000000/(16*9600) = 19.53125
    // UCBR  = 19
    // UCBRF = .53125 * 16 = 8 (0.5) (Remainder of the divide)
    // UCBRS = 3000000/9600 remainder=0.5 -> 0xAA (look up table 22-4)
    EUSCI_A0->BRW = 19;
    EUSCI_A0->MCTLW = 0xAA81;//found above

    EUSCI_A0->CTLW0 &= ~BIT0;  // Enable EUSCI
    EUSCI_A0->IFG &= ~BIT0;    // Clear interrupt
    EUSCI_A0->IE |= BIT0;      // Enable interrupt
    NVIC_EnableIRQ(EUSCIA0_IRQn);
}


