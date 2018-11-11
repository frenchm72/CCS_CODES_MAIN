/****************************************************************************
  Title:       EGR 226 Project I
  Filename:    Project I
  Author(s):   Mathew Yerian-French
  Date:        10/10/2018
  Instructor:  Professor Scott Zuidema
  Description:
  Note:
*****************************************************************************/

#include "msp.h"                                                                                                                    //msp header file
#include "LCD.h"                                                                                                                    //lcd header file from previous lab
#include "motor.h"                                                                                                                  //motor header file from previous lab
#include "keypad.h"                                                                                                                 //keypad header file from previous file
#include "led.h"                                                                                                                    //led header file from previous lab
#include "promain.h"                                                                                                                //project main file for this project
#include <string.h>                                                                                                                 //string file
#include <stdio.h>                                                                                                                  //standard I/O file
#include <stdlib.h>                                                                                                                 //standard library file
#include <math.h>                                                                                                                   //math file

uint16_t  keyS = 0x0000;                                                                                                            //declares some global variables for keypad
char new, key[3];                                                                                                                   //more variables for the keypad
int i, a, p, open_close, sleep, lightOnOff = 0, emerg = 0;                                                                          //global variables used though out the program
float redBright = 7499.0, blueBright = 7499.0, greenBright = 7499.0, motoSpd = 0.0;

enum states
{
  MAIN_MENU, DOOR_MENU, MOTOR_MENU, LIGHT_MENU,                                                                                     //main menus states

  DOOR_OPEN, DOOR_CLOSE,                                                                                                            //door sub menu states

  RED_LIGHT, BLUE_LIGHT, GREEN_LIGHT,                                                                                               //light sub menu states

  MOTOR_SPEED,                                                                                                                      //sub motor menu states

  EMERGENCY, SLEEP, LIGHT_ON, LIGHT_OFF                                                                                             //extra states

}state;

void main(void)

{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		                                                                                // stop watchdog timer
	__disable_irq();                                                                                                                //disables irq for setup
	NVIC -> ISER[1] = 0x00000008 | 1 <<((PORT1_IRQn)&31);                                                                           //this is for the timers as well as the interrupt
	resetKey();                                                                                                                     //makes the key char all null to start for the keypad
    StsTick_Init();                                                                                                                 //starts the systick timer
    delay_micro(100);                                                                                                               //a delay to start up
	initLCDPins();                                                                                                                  //inits the pins for the LCD
	initLEDpins();                                                                                                                  //inits the pins for LEDs
	delay_micro(100);                                                                                                               //lets the stuff before the start up
	initLCD();                                                                                                                      //starts up the LCD
    delay_micro(100);                                                                                                               //delay to help with the screen
	initMOTOpins();                                                                                                                 //inits the motor pins
	initTimerAmoto();                                                                                                               //inits timer a for the motor as well as the servo
	initTimerALED();                                                                                                                //inits timer a for the LEDs
	KeyInit();                                                                                                                      //key pad init
	state = MAIN_MENU;                                                                                                              //sets the inital state to the main menu
    p = 0;                                                                                                                          //print variable to 0 so it can initially print
    open_close = 0;                                                                                                                 //zero means closed to set the change variable for the servo
    setGreenPWM(greenBright);                                                                                                       //sets the brightness of the the green led to 100 percent to begin
    setBluePWM(blueBright);                                                                                                         //sets the brightness of the the blue led to 100 percent to begin
    setRedPWM(redBright);                                                                                                           //sets the brightness of the the red led to 100 percent to begin
    setMotoPWM(motoSpd);                                                                                                            //sets the motor speed to init 0 percent to begin
    onServoPWM();                                                                                                                   //function to turn on servo
    setServoClosePWM();                                                                                                             //sets the servo to the closed position
    delay_ms(500);                                                                                                                  //delay to move position
    offServoPWM();                                                                                                                  //turns off the servo
    initMotoPWM();                                                                                                                  //inits the motor PWM
    delay_ms(500);                                                                                                                  //delays for the set ups before***helps with LCD
    __enable_irq();                                                                                                                 //after all the set up it initializes the irq to enable interrupts

	char mainMenu[10] = {'M','e','n','u',},                                                                                         //main menu display letters for header of menu
	     mainDoor[7] = {'1',')',' ','D','o','o','r',},                                                                              //door option
	     mainMotor[8] = {'2',')',' ','M','o','t','o','r',},                                                                         //motor option
	     mainLight[9] = {'3',')',' ','L','i','g','h','t','s',};                                                                     //light option

	char bootMenu[16] = {'B','O','O','T','I','N','G','.','.','.','.','.','.','.','.','.',};                                         //booting menu

    char doorMenu[4] = {'D','o','o','r',},                                                                                          //main door display letters   for header of menu
         doorOpen[12] = {'1',')',' ','O','p','e','n',' ','D','o','o','r',},                                                         //option to open the door
         doorClose[13] = {'2',')',' ','C','l','o','s','e',' ', 'D','o','o','r',},                                                   //option the close the door
         opening[16] = {'O','p','e','n','i','n','g','.','.','.','.','.','.','.','.','.',},                                          //animation opening just for display
         closeing[16] = {'C','l','o','s','i','n','g','.','.','.','.','.','.','.','.','.',},                                         //animation closing just for display
         allOpen[16] = {'I','t','s',' ','A','l','r','e','a','d','y',' ','O','p','e','n',},                                          //in case its already open
         allClosed[18] = {'I','t','s',' ','A','l','r','e','a','d','y',' ','C','l','o','s','e','d',};                                //in case its already closed

    char lightMenu[5] = {'L','i','g','h','t',},                                                                                     //main light display letters for header of menu
         lightGreen[12] = {'1',')',' ','G','r','e','e','n',' ','L','E','D',},                                                       //green light option
         lightBlue[11] = {'2',')',' ','B','l','u','e',' ','L','E','D',},                                                            //blue light option
         lightRed[10] = {'3',')',' ','R','e','d',' ','L','E','D',};                                                                 //red light option

    char motoMenu[5] = {'M','o','t','o','r',},                                                                                      //motor screen speed selector header
         motoSpeed[25] = {'P','l','e','a','s','e',' ','E','n','t','e','r',' ','A',' ','S','p','e','e','d',' ','i','n',' ','%',},    //Prompting for user to enter a speed
         success[16] = {'S','U','C','C','E','S','S','.','.','.','.','S','A','V','E','D',};                                          //animation to say it saved

    char sleepMenu[21] = {'P','r','e','s','s',' ','*',' ','T','o',' ','E','x','i','t',' ','S','l','e','e','p',};                    //for the sleep screen

    char greenMenu[5] = {'G','r','e','e','n',},                                                                                     //green screen selector
         blueMenu[4] = {'B','l','u','e',},                                                                                          //blue screen selector
         redMenu[3] = {'R','e','d',},                                                                                               //red screen selector
         brightMenu[25] = {'P','l','e','a','s','e',' ','E','n','t','e','r',' ','B','r','i','g','h','t','n','e','s','s',' ','%',};   //to prompt the user to enter a brightness in a percentage

    char lightsOn[9] = {'L','i','g','h','t','s',' ','O','N',},                                                                      //to tell the user they just turned on the lights
         lightsOff[10] =  {'L','i','g','h','t','s',' ','O','F','F',};                                                               //to tell the user they just turned off the lights

    char emergencyMenu[9] = {'E','m','e','r','g','e','n','c','y',},                                                                 //emergency header
         emerExit[33] = {'P','r','e','s','s',' ','E','m','e','r','g','e','n','c','y',' ', 'B','u','t','t','o','n',' ','T','o',      //tells the user what to do to start back up
                         ' ','R','e','s','t','a','r','t',};                                                                         //tells the user what to do to start back up

                comWrit(0x0C);                                                                                                      //turns off cursor
                comWrit(0xC0);                                                                                                      //starts cursor at top left corner/ the first address
                delay_micro(100);                                                                                                   //delay for the command
                for(i=0;i<7;i++)                                                                                                    //for loop to print boot menu to the LCD
                {
                    dataWrit(bootMenu[i]);                                                                                          //data write function to write the screen
                }
                for(i=7;i<16;i++)                                                                                                   //for loop for boot
                {
                        dataWrit(bootMenu[i]);                                                                                      //data write function to write the screen
                        delay_ms(500);                                                                                              //to make a little animation
                }
                comWrit(0x01);  delay_micro(100);                                                                                   //clears LCD

	while(1)                                                                                                                        //while loop for the whole program
	{                                                                                                                               //BEGINING OF THE WHILE
	    switch(state)                                                                                                               //switch statement for the states
	    {                                                                                                                           //BEGINE OF THE SWITCH
	    case MAIN_MENU:                                                                                                             //BEGINING OF MAIN MENU STATE
	        butPres();                                                                                                              //to check if the light button has been pressed
	        if(p==0)                                                                                                                //to make it print
	        {
	            delay_ms(100);                                                                                                      //to clear up the LCD
	            comWrit(0x86);                                                                                                      //Beginning of first line in the middle
	            delay_micro(100);                                                                                                   //command delay
	                for(i=0;i<4;i++)                                                                                                //main menu header
	                {
	                        dataWrit(mainMenu[i]);                                                                                  //data write function to write the screen
	                    }
	            comWrit(0xC0);                                                                                                      //second row then prints the 1) selector
	            delay_micro(100);                                                                                                   //delay for the command
	                for(i=0;i<7;i++)                                                                                                //for the door option
	                    {
	                        dataWrit(mainDoor[i]);                                                                                  //data write function to write the screen
	                    }
	            comWrit(0x90);                                                                                                      //third row for 2 selector
	            delay_micro(100);                                                                                                   //delay for command
	                for(i=0;i<8;i++)                                                                                                //for the motor selection
	                    {
	                        dataWrit(mainMotor[i]);                                                                                 //data write function to write the screen
	                    }
	            comWrit(0xD0);                                                                                                      //fourth row for 3 selector
	            delay_micro(100);                                                                                                   //delay for command
	                for(i=0;i<9;i++)                                                                                                //for the light selector
	                    {
	                        dataWrit(mainLight[i]);                                                                                 //data write function to write the screen
	                    }
	            p=1;                                                                                                                //to say it has printed
	        }
	        getKey();
	        if(new == '1') {state = DOOR_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;             }                   //to change to door state
	        if(new == '2') {state = MOTOR_MENU; comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;  resetKey();}                   //to change to motor state
	        if(new == '3') {state = LIGHT_MENU; comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;             }                   //to change to light state
	        break;                                                                                                                  //END OF THE MAIN MENU



	    case DOOR_MENU:                                                                                                             //BEGINIGN OF THE DOOR MENU
	        butPres();                                                                                                              //to check if the light button has been pressed
	        if(p==0)
            {
	                delay_ms(100);                                                                                                  //Allowing stuff to catch up with the display
	                comWrit(0x86);                                                                                                  //starts the cursor in a position to display the door menu header
	                delay_micro(100);                                                                                               //delay for the command write function
	                for(i=0;i<4;i++)                                                                                                //for statement to display the door menu header
	                {
	                    dataWrit(doorMenu[i]);                                                                                      //data write function to write the screen
	                }
	                comWrit(0xC0);                                                                                                  //second row then prints the 1) selector
	                delay_micro(100);                                                                                               //delay for the command write function
	                for(i=0;i<12;i++)                                                                                               //displays the first selector
	                {
	                        dataWrit(doorOpen[i]);                                                                                  //data write function to write the screen
	                }
	                comWrit(0x90);                                                                                                  //third row for 2 selector
	                delay_micro(100);                                                                                               //delay for the command write function
	                for(i=0;i<13;i++)                                                                                               //this is to print the door close option
	                {
	                        dataWrit(doorClose[i]);                                                                                 //data write function to write the screen
	                }
	                comWrit(0x0C);                                                                                                  //turns off cursor
	                p=1;                                                                                                            //sets print variable to 1 so it doesn't print again
            }
	        getKey();                                                                                                               //gets the key input
            if(new == '1') {state = DOOR_OPEN;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;  }                              //Basically if the 1 key is pressed it clears the screen and changes state. the print variable and sleep variable are set to 0 because something has been done
            if(new == '2') {state = DOOR_CLOSE; comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;  }                              //Basically if the 2 key is pressed it clears the screen and changes state. the print variable and sleep variable are set to 0 because something has been done
            if(new == '*') {state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;  }                              //Basically if the # key is pressed it clears the screen and changes state.to go to the main menu the print variable and sleep variable are set to 0 because something has been done
	        break;                                                                                                                  //END OF THE DOOR MENU



	    case MOTOR_MENU:                                                                                                            //BEGINING OF THE MOTOR MENU
	        butPres();                                                                                                              //to check if the light button has been pressed
            if(p==0)                                                                                                                //if the print variable is 0 it'll print
            {
                       delay_ms(100);
                       comWrit(0x86);                                                                                               //cursor set to make the header for the motor menu
                       delay_micro(100);                                                                                            //delay for the command write function
                       for(i=0;i<5;i++)                                                                                             //to write the letter for the motor
                       {
                           dataWrit(motoMenu[i]);                                                                                   //data write function to write the screen
                       }
                       comWrit(0xC0);                                                                                               //first sentence to display what to do for the user
                       delay_micro(100);                                                                                            //delay for the command write function
                       for(i=0;i<15;i++)                                                                                            //the for loop to create this sentence
                       {
                           dataWrit(motoSpeed[i]);                                                                                  //data write function to write the screen
                       }
                       comWrit(0x90);                                                                                               //third row for the rest of the sentence
                       delay_micro(100);                                                                                            //delay for the command write function
                       for(i=15;i<25;i++)                                                                                           //to write the string thats the rest of the sentence
                       {
                           dataWrit(motoSpeed[i]);                                                                                  //data write function to write the screen
                       }
                       for(i=0;i<3;i++)                                                                                             //for when they key is pressed and displays the values
                       {
                           dataWrit(key[i]);                                                                                        //data write function to write the screen
                       }
                       p=1;                                                                                                         //sets the print variable to 1 so it doesn't keep printing
            }
	        getKey();                                                                                                               //gets the key from the keypad
	        if(new == '*') {state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;  a = 0;  break;}                //if the * is pressed it goes back to the main menu
	        if(new == '#')                                                                                                          //if statement for the # key for when the user enter less than 3 numbers
	        {
	            convertINTmoto();                                                                                                   //function that converts the string from the keypad to an actual number
	            setMotoPWM(motoSpd);                                                                                                //this then sets that speed that was enter to the motor PWM
	            resetKey();                                                                                                         //resets the key string to all \0
	            comWrit(0xD0);                                                                                                      //for the saved animation on the final row
	            delay_micro(100);                                                                                                   //delay for the command write function
	            for(i=0;i<16;i++)                                                                                                   //the loop for the saved string
	            {
	                    dataWrit(success[i]);                                                                                       //data write function to write the screen
	            }
	            delay_ms(2000);                                                                                                     //delay for the display to be read
	            state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;                                             //changes state clears display and sets print and sleep variable 0
	            a = 0;                                                                                                              //a = to zero this is the number of strings that got entered
	            break;                                                                                                              //breaks out of the state so it doesn't go anywhere else
	        }
	        if(a == 3)                                                                                                              //if the user enters 3 pin in to the keypad for the duty cycle
            {
                convertINTmoto();                                                                                                   //function that converts the string from the keypad to an actual number
                setMotoPWM(motoSpd);                                                                                                //this then sets that speed that was enter to the motor PWM
                resetKey();                                                                                                         //resets the key string to all \0
                comWrit(0xD0);                                                                                                      //for the saved animation on the final row
                delay_micro(100);                                                                                                   //delay for the command write function
                for(i=0;i<16;i++)                                                                                                   //the loop for the saved string
                {
                        dataWrit(success[i]);                                                                                       //data write function to write the screen
                }
                delay_ms(2000);                                                                                                     //delay for the display to be read
                state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;                                             //changes state clears display and sets print and sleep variable 0
                a = 0;                                                                                                              //a = to zero this is the number of strings that got entered
                break;                                                                                                              //breaks out of the state so it doesn't go anywhere else
            }
            if(keyS != 0)                                                                                                           //to write what was entered in the key pad to a string
                {
                    key[0] = key[1];                                                                                                //Shifts 1 to the 0 element
                    key[1] = key[2];                                                                                                //shifts 2 to the 1st element
                    key[2] = new;                                                                                                   //shifts the new char in to the string
                    a++;                                                                                                            //adds 1 to a to indicate the keypad was pressed
                    p=0;                                                                                                            //changes this to 0 so it will print the string
                }
	        break;                                                                                                                  //END OF MOTOR MENU/ CYCLE



	    case LIGHT_MENU:                                                                                                            //BEGINING OF THE LIGHTS MENU
	        butPres();                                                                                                              //to check if the light button has been pressed
	        if(p==0)                                                                                                                //if the print variable is 0 it'll print
	        {
	                    delay_ms(100);                                                                                              //delay to clear up LCD
	                    comWrit(0x86);                                                                                              //light header
	                    delay_micro(100);                                                                                           //delay for the command write function
	                    for(i=0;i<5;i++)                                                                                            //for loop to make the header for lights
	                    {
	                        dataWrit(lightMenu[i]);                                                                                 //data write function to write the screen
	                    }
	                    comWrit(0xC0);                                                                                              //starts on the second line
	                    delay_micro(100);                                                                                           //delay for the command write function
	                    for(i=0;i<12;i++)                                                                                           //for the green opt for the LEDs
	                    {
	                        dataWrit(lightGreen[i]);                                                                                //Writes each character from the string
	                    }
	                    comWrit(0x90);                                                                                              //third row for 2 option
	                    delay_micro(100);                                                                                           //delay for the command write function
	                    for(i=0;i<11;i++)                                                                                           //for loop for the second option thats blue LED
	                    {
	                        dataWrit(lightBlue[i]);                                                                                 //Writes each character from the string
	                    }
	                    comWrit(0xD0);                                                                                              //fourth row for 3 option
	                    delay_micro(100);                                                                                           //delay for the command write function
	                    for(i=0;i<10;i++)                                                                                           //this is the for loop to display the red option
	                    {
	                        dataWrit(lightRed[i]);                                                                                  //Writes each character from the string
	                    }
	                    comWrit(0x0C);                                                                                              //turns of the cursor
	                    p=1;                                                                                                        //Turns the print variable to 1 so i doesn't keep printing
	        }
	        getKey();                                                                                                               //get the key that is entered
            if(new == '1') {state = GREEN_LIGHT; comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;   }                            //Changes the state if pressed 1 to green and does a clear of variables
            if(new == '2') {state = BLUE_LIGHT;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;   }                            //changes the state if 2 is pressed to blue and clears variables
            if(new == '3') {state = RED_LIGHT;   comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;   }                            //changes the state if 3 is pressed to red and clears variables
            if(new == '*') {state = MAIN_MENU;   comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;   }                            //if * is pressed the state changes back to main menu and clears variables
	        break;                                                                                                                  //END OF LIGHT STATE



	    case DOOR_OPEN:                                                                                                             //START OF THE DOOR OPENING STATE
	        butPres();                                                                                                              //to check if the light button has been pressed
            if(open_close == 1)                                                                                                     //checks if the door has been opened
            {
                if(p==0)                                                                                                            //if the print variable is 0 it'll print
                {
                    delay_ms(100);                                                                                                  //delay to clear up LCD issues
                    comWrit(0x86);                                                                                                  //Beginning of first line but needs to go to middle to then write to the middle of the screen
                    delay_micro(100);                                                                                               //delay for the command write function
                    for(i=0;i<4;i++)                                                                                                //to display this is with in the door menu
                    {
                        dataWrit(doorMenu[i]);                                                                                      //data write function to write the screen
                    }
                    comWrit(0xC0);                                                                                                  //start of third line
                    delay_micro(100);                                                                                               //delay for the command write function
                    for(i=0;i<12;i++)                                                                                               //displays already open
                    {
                        dataWrit(allOpen[i]);                                                                                       //data write function to write the screen
                    }
                    comWrit(0x90);                                                                                                  //THIRD LINE
                    delay_micro(100);                                                                                               //delay for the command write function
                    for(i=12;i<16;i++)                                                                                              //rest of the already open string
                    {
                        dataWrit(allOpen[i]);                                                                                       //data write function to write the screen
                    }
                    p=1;                                                                                                            //sets print variable to 1 to not print again
                }
                delay_ms(1000);                                                                                                     //delay to allow user to read it
                state = DOOR_MENU;    comWrit(0x01);  delay_micro(100);  p=0;                                                       //sets state back to door menu because the door is already open
                break;                                                                                                              //breaks out of the open
            }
	             if(open_close == 0)                                                                                                //if door is closed now open
	             {
	                 DOOR_C_PORT -> OUT &= ~DOOR_C_PIN;                                                                             //close led off
	                 DOOR_O_PORT -> OUT |= DOOR_O_PIN;                                                                              //open led on
	                 onServoPWM();                                                                                                  //turns on the servo
	                 setServoOpenPWM();                                                                                             //sets to the open position
	                 if(p==0)                                                                                                       //allows it to print because p should = 0
	                 {
	                 delay_ms(100);                                                                                                 //clears up display
	                 comWrit(0x86);                                                                                                 //Beginning of first line but needs to go to middle to then write to the middle of the screen
	                 delay_micro(100);                                                                                              //delay for the command write function
	                 for(i=0;i<4;i++)                                                                                               //door menu header
	                 {
	                     dataWrit(doorMenu[i]);                                                                                     //data write function to write the screen
	                 }
	                 comWrit(0xC0);                                                                                                 //second line
	                 delay_micro(100);                                                                                              //delay for the command write function
	                 for(i=0;i<7;i++)                                                                                               //displays opening
	                 {
	                     dataWrit(opening[i]);                                                                                      //string for opening
	                 }
	                 for(i=7;i<16;i++)                                                                                              //animation loop
	                 {
	                     dataWrit(opening[i]);                                                                                      //dots to incite its doing something
	                     delay_ms(250);                                                                                             //delay to look like its loading
	                 }
	                 p=1;                                                                                                           //Turns the print variable to 1 so i doesn't keep printing
	                 }
	                 open_close = 1;                                                                                                //sets the open close variable to 1 saying its open
	                 offServoPWM();                                                                                                 //turns off the servo
	             }
	        state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p = 0;                                                           //changes the state back to the main menu
	        break;



	    case DOOR_CLOSE:                                                                                                            //START OF THE DOOR CLOSING
	        butPres();                                                                                                              //to check if the light button has been pressed
            if(open_close == 0)                                                                                                     //checks if the door has been previously closed or opened
            {
                if(p==0)                                                                                                            //if the print variable is 0 it'll print
                {
                    delay_ms(100);                                                                                                  //delays to clear up the LCD
                    comWrit(0x86);                                                                                                  //Beginning of first line but needs to go to middle to then write to the middle of the screen
                    delay_micro(100);                                                                                               //delay for the command write function
                    for(i=0;i<4;i++)                                                                                                //to write the header for door
                    {
                        dataWrit(doorMenu[i]);                                                                                      //data write function to write the screen
                    }
                    comWrit(0xC0);                                                                                                  //second row then prints already closed
                    delay_micro(100);                                                                                               //delay for the command write function
                    for(i=0;i<12;i++)
                    {
                        dataWrit(allClosed[i]);                                                                                     //data write function to write the screen
                    }
                    comWrit(0x90);                                                                                                  //second row then prints the 1) selector
                    delay_micro(100);                                                                                               //delay for the command write function
                    for(i=12;i<18;i++)                                                                                              //displays the rest of already closed
                    {
                        dataWrit(allClosed[i]);                                                                                     //data write function to write the screen
                    }
                    p=1;                                                                                                            //Turns the print variable to 1 so i doesn't keep printing
                }
                delay_ms(1000);                                                                                                     //time allows user to read the display
                state = DOOR_MENU;   comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;                                            //reverts it back to door menu because its already been closed
                break;                                                                                                              //breaks out of this to stop the program from continuing
            }
	        if(open_close == 1)                                                                                                     //this checks if the door hasn't been closed
	        {
	            DOOR_C_PORT -> OUT |= DOOR_C_PIN;                                                                                   //turns the closed light on
	            DOOR_O_PORT -> OUT &= ~DOOR_O_PIN;                                                                                  //turns the open light off
	            onServoPWM();                                                                                                       //turns on the servo pin
	            setServoClosePWM();                                                                                                 //sets the servo to closed
	            if(p==0)                                                                                                            //if the print variable is 0 it'll print
	            {
	                delay_ms(100);                                                                                                  //delay to clear up LCD
	                comWrit(0x86);                                                                                                  //Beginning of first line but needs to go to middle to then write to the middle of the screen
	                delay_micro(100);                                                                                               //delay for the command write function
	                for(i=0;i<4;i++)                                                                                                //for the door menu header
	                {
	                    dataWrit(doorMenu[i]);                                                                                      //data write function to write the screen
	                }
	                comWrit(0xC0);                                                                                                  //second row then prints the 1) selector
	                delay_micro(100);                                                                                               //delay for the command write function
	                for(i=0;i<7;i++)                                                                                                //Closing animation
	                {
	                    dataWrit(closeing[i]);                                                                                      //data write function to write the screen
	                }
	                for(i=7;i<16;i++)                                                                                               //for the res of the closing animation
	                {
	                    dataWrit(closeing[i]);                                                                                      //data write function to write the screen
	                    delay_ms(250);                                                                                              //delay to make an animation
	                }
	                p=1;                                                                                                            //Turns the print variable to 1 so i doesn't keep printing
	            }
	            open_close = 0;                                                                                                     //sets the close variable to 0 meaning its closed
	            offServoPWM();                                                                                                      //finally turns off the servo
	        }
	        state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p = 0;  sleep = 0;                                               //sets to main menu and clears screen
	        break;                                                                                                                  //END OF DOOR CLOSING STATE



        case GREEN_LIGHT:                                                                                                           //BEGINING OF GREEN LIGHT MENU
            butPres();                                                                                                              //to check if the light button has been pressed
            if(p==0)                                                                                                                //if the print variable is 0 it'll print
            {
            delay_ms(100);                                                                                                          //delay to clear up LCD
            comWrit(0x86);                                                                                                          //Beginning of first line but needs to go to middle to then write to the middle of the screen
            delay_micro(100);                                                                                                       //delay for command
            for(i=0;i<5;i++)                                                                                                        //for the green header menu
            {
                dataWrit(greenMenu[i]);                                                                                             //data write function to write the screen
            }
            comWrit(0xC0);                                                                                                          //second row then asked user to enter a %
            delay_micro(100);                                                                                                       //delay for command
            for(i=0;i<13;i++)                                                                                                       //to ask user for the % of brightness
            {
                dataWrit(brightMenu[i]);                                                                                            //data write function to write the screen
            }
            comWrit(0x90);                                                                                                          //third row for the rest of the prompt
            delay_micro(100);                                                                                                       //delay for the command
            for(i=13;i<25;i++)                                                                                                      //displays the rest of the string
            {
                dataWrit(brightMenu[i]);                                                                                            //data write function to write the screen
            }
            for(i=0;i<3;i++)                                                                                                        //displays the keys entered if any
            {
                dataWrit(key[i]);                                                                                                   //data write function to write the screen
            }
            p=1;                                                                                                                    //Turns the print variable to 1 so i doesn't keep printing
            }
            getKey();                                                                                                               //gets a new key for the string
            if(new == '*') {state = LIGHT_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;  resetKey();  a = 0;  break;}  //if this key is pressed it goes back
            if(new == '#')                                                                                                          //if key is pressed it takes the last keys that were entered
            {
                convertINTgreen();                                                                                                  //converts the key string to an int
                setGreenPWM(greenBright);                                                                                           //sends the PWM
                resetKey();                                                                                                         //resets the key to null so it doesn't print
                comWrit(0xD0);                                                                                                      //Command write for successful save
                delay_micro(100);                                                                                                   //delay for the command write function
                for(i=0;i<16;i++)                                                                                                   //for loop for the successful save
                {
                    dataWrit(success[i]);                                                                                           //data write function to write the screen
                }
                delay_ms(1000);                                                                                                     //delays to allow user to read display
                state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;                                             //clears display and returns to main menu
                a = 0;                                                                                                              //resets a for the keys entered
                break;                                                                                                              //breaks out of this so it doesn't continue
            }
            if(a == 3)                                                                                                              //if the user has entered 3 keys
            {
                convertINTgreen();                                                                                                  //converts the key string to an int
                setGreenPWM(greenBright);                                                                                           //sends the PWM
                resetKey();                                                                                                         //resets the key to null so it doesn't print
                comWrit(0xD0);                                                                                                      //Command write for successful save
                delay_micro(100);                                                                                                   //delay for the command write function
                for(i=0;i<16;i++)                                                                                                   //for loop for the successful save
                {
                    dataWrit(success[i]);                                                                                           //data write function to write the screen
                }
                delay_ms(1000);                                                                                                     //delays to allow user to read display
                state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;                                             //clears display and returns to main menu
                a = 0;                                                                                                              //resets a for the keys entered
                break;                                                                                                              //breaks out of this so it doesn't continue
            }
            if(keyS != 0)                                                                                                           //if the key has been pressed
            {
                key[0] = key[1];                                                                                                    //Shifts 1 to the 0 element
                key[1] = key[2];                                                                                                    //shifts 2 to the 1st element
                key[2] = new;                                                                                                       //shifts the new char in to the string
                a++;                                                                                                                //adds 1 to a to indicate the keypad was pressed
                p=0;                                                                                                                //changes this to 0 so it will print the string
            }
            break;                                                                                                                  //END OF GREEN STATE



        case BLUE_LIGHT:                                                                                                            //BEGINING OF BLUE STATE
            butPres();                                                                                                              //to check if the light button has been pressed
            if(p==0)                                                                                                                //if the print variable is 0 it'll print
            {
                delay_ms(100);                                                                                                      //delay to clear up LCD
                comWrit(0x86);                                                                                                      //Beginning of first line but needs to go to middle to then write to the middle of the screen
                delay_micro(100);                                                                                                   //delay for the command write function
                for(i=0;i<4;i++)                                                                                                    //header for menu
                {
                    dataWrit(blueMenu[i]);                                                                                          //data write function to write the screen
                }
                comWrit(0xC0);                                                                                                      //second row
                delay_micro(100);                                                                                                   //delay for the command write function
                for(i=0;i<13;i++)                                                                                                   //brightness prompt
                {
                    dataWrit(brightMenu[i]);                                                                                        //data write function to write the screen
                }
                comWrit(0x90);                                                                                                      //third row for the rest of the prompt
                delay_micro(100);                                                                                                   //delay for the command write function
                for(i=13;i<25;i++)                                                                                                  //the rest of the prompt
                {
                    dataWrit(brightMenu[i]);                                                                                        //data write function to write the screen
                }
                for(i=0;i<3;i++)                                                                                                    //displays keys entered if any
                {
                    dataWrit(key[i]);                                                                                               //data write function to write the screen
                }
                p=1;                                                                                                                //Turns the print variable to 1 so i doesn't keep printing
            }
            getKey();                                                                                                               //gets the key from the user
            if(new == '*') {state = LIGHT_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;  resetKey();  a = 0;  break;}  //if this key is pressed it goes back
            if(new == '#')                                                                                                          //if key is pressed it takes the last keys that were entered
            {
                convertINTblue();                                                                                                   //converts the key string to an int
                setBluePWM(blueBright);                                                                                             //sends the PWM
                resetKey();                                                                                                         //resets the key to null so it doesn't print
                comWrit(0xD0);                                                                                                      //Command write for successful save
                delay_micro(100);                                                                                                   //delay for the command write function
                for(i=0;i<16;i++)                                                                                                   //for loop for the successful save
                {
                    dataWrit(success[i]);                                                                                           //data write function to write the screen
                }
                delay_ms(1000);                                                                                                     //delays to allow user to read display
                state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;                                             //clears display and returns to main menu
                a = 0;                                                                                                              //resets a for the keys entered
                break;                                                                                                              //breaks out of this so it doesn't continue
            }
            if(a == 3)                                                                                                              //if the user has entered 3 keys
            {
                convertINTblue();                                                                                                   //converts the key string to an int
                setBluePWM(blueBright);                                                                                             //sends the PWM
                resetKey();                                                                                                         //resets the key to null so it doesn't print
                comWrit(0xD0);                                                                                                      //Command write for successful save
                delay_micro(100);                                                                                                   //delay for the command write function
                for(i=0;i<16;i++)                                                                                                   //for loop for the successful save
                {
                    dataWrit(success[i]);                                                                                           //data write function to write the screen
                }
                delay_ms(1000);                                                                                                     //delays to allow user to read display
                state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;                                             //clears display and returns to main menu
                a = 0;                                                                                                              //resets a for the keys entered
                break;                                                                                                              //breaks out of this so it doesn't continue
            }
            if(keyS != 0)                                                                                                           //if the key has been pressed
            {
                key[0] = key[1];                                                                                                    //Shifts 1 to the 0 element
                key[1] = key[2];                                                                                                    //shifts 2 to the 1st element
                key[2] = new;                                                                                                       //shifts the new char in to the string
                a++;                                                                                                                //adds 1 to a to indicate the keypad was pressed
                p=0;                                                                                                                //changes this to 0 so it will print the string
            }
            break;                                                                                                                  //END OF BLUE STATE



	    case RED_LIGHT:                                                                                                             //BEGINING OF RED STATE
	        butPres();                                                                                                              //to check if the light button has been pressed
	        if(p==0)                                                                                                                //if the print variable is 0 it'll print
	        {
	            delay_ms(100);                                                                                                      //delay to clear up LCD
	            comWrit(0x85);                                                                                                      //begininG of first line but needs to go to middle to then write to the middle of the screen
	            delay_micro(100);                                                                                                   //delay for the command write function
	            for(i=0;i<3;i++)                                                                                                    //header for menu
	            {
	                dataWrit(redMenu[i]);                                                                                           //data write function to write the screen
	            }
	            comWrit(0xC0);                                                                                                      //second row then prints prompt
	            delay_micro(100);                                                                                                   //delay for the command write function
	            for(i=0;i<13;i++)                                                                                                   //brightness prompt
	            {
	                dataWrit(brightMenu[i]);                                                                                        //data write function to write the screen
	            }
	            comWrit(0x90);                                                                                                      //third row
	            delay_micro(100);                                                                                                   //delay for the command write function
	            for(i=13;i<25;i++)                                                                                                  //brightness prompt
	            {
	                dataWrit(brightMenu[i]);                                                                                        //data write function to write the screen
	            }
	            for(i=0;i<3;i++)                                                                                                    //displays keys entered if any
	            {
	                dataWrit(key[i]);                                                                                               //data write function to write the screen
	            }
	            p=1;                                                                                                                //Turns the print variable to 1 so i doesn't keep printing
	        }
	        getKey();                                                                                                               //gets key from user
	        if(new == '*') {state = LIGHT_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;  resetKey();  a = 0;  break;}  //if this key is pressed it goes back
	        if(new == '#')                                                                                                          //if key is pressed it takes the last keys that were entered
	        {
	            convertINTred();                                                                                                    //converts the key string to an int
	            setRedPWM(redBright);                                                                                               //sends the PWM
	            resetKey();                                                                                                         //resets the key to null so it doesn't print
	            comWrit(0xD0);                                                                                                      //Command write for successful save
	            delay_micro(100);                                                                                                   //delay for the command write function
	            for(i=0;i<16;i++)                                                                                                   //for loop for the successful save
	            {
	                dataWrit(success[i]);                                                                                           //data write function to write the screen
	            }
	            delay_ms(1000);                                                                                                     //delays to allow user to read display
	            state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;                                             //clears display and returns to main menu
	            a = 0;                                                                                                              //resets a for the keys entered
	            break;                                                                                                              //breaks out of this so it doesn't continue
	        }
	        if(a == 3)                                                                                                              //if the user has entered 3 keys
	        {
	            convertINTred();                                                                                                    //converts the key string to an int
	            setRedPWM(redBright);                                                                                               //sends the PWM
	            resetKey();                                                                                                         //resets the key to null so it doesn't print
	            comWrit(0xD0);                                                                                                      //Command write for successful save
	            delay_micro(100);                                                                                                   //delay for the command write function
	            for(i=0;i<16;i++)                                                                                                   //for loop for the successful save
	            {
	                dataWrit(success[i]);                                                                                           //data write function to write the screen
	            }
	            delay_ms(1000);                                                                                                     //delays to allow user to read display
	            state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;                                             //clears display and returns to main menu
	            a = 0;                                                                                                              //resets a for the keys entered
	            break;                                                                                                              //breaks out of this so it doesn't continue
	        }
	        if(keyS != 0)                                                                                                           //if the key has been pressed
	        {
	            key[0] = key[1];                                                                                                    //Shifts 1 to the 0 element
	            key[1] = key[2];                                                                                                    //shifts 2 to the 1st element
	            key[2] = new;                                                                                                       //shifts the new char in to the string
	            a++;                                                                                                                //adds 1 to a to indicate the keypad was pressed
	            p=0;                                                                                                                //changes this to 0 so it will print the string
	        }
	        break;                                                                                                                  //END OF THE RED STATE



	    case LIGHT_ON:                                                                                                              //BEGINING OF THE LIGHT TURNING ON STATE
	        p = 0;                                                                                                                  //print variable to zero to print
	        initGreenPWM();                                                                                                         //inits greens PWM
	        initBluePWM();                                                                                                          //inits blue PWM
	        initRedPWM();                                                                                                           //inits red PWM
	        lightOnOff = 1;                                                                                                         //sets light flag
	        comWrit(0x01);                                                                                                          //clears screen
	        delay_micro(100);                                                                                                       //delay for the command write function
	        if(p==0)                                                                                                                //if the print variable is 0 it'll print
	        {
	            delay_ms(100);                                                                                                      //delay to clear up LCD
	            comWrit(0x86);                                                                                                      //fist line for header
	            delay_micro(100);                                                                                                   //delay for the command write function
	            for(i=0;i<5;i++)                                                                                                    //header for menu
	            {
	                dataWrit(lightMenu[i]);                                                                                         //data write function to write the screen
	            }
	            comWrit(0xC0);                                                                                                      //second row
	            delay_micro(100);                                                                                                   //delay for the command write function
	            for(i=0;i<9;i++)                                                                                                    //lights on for loop
	            {
	                dataWrit(lightsOn[i]);                                                                                          //data write function to write the screen
	            }
	            comWrit(0x0C);                                                                                                      //command for the LCD
	            delay_ms(500);                                                                                                      //delay for the command write function and to read
	            p=1;                                                                                                                //Turns the print variable to 1 so i doesn't keep printing
	        }
	        state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;                                                 //clears display and returns to main menu
	        break;                                                                                                                  //END OF LIGHTS TURNING ON STATE



	    case LIGHT_OFF:                                                                                                             //BEGINING OF THE LIGHT TURNING OFF STATE
	        p = 0;                                                                                                                  //print variable to zero
	        offGreenPWM();                                                                                                          //turns off green PWM
	        offBluePWM();                                                                                                           //turns off blue PWM
	        offRedPWM();                                                                                                            //turns off red PWM
	        lightOnOff = 0;                                                                                                         //sets light flag
	        comWrit(0x01);                                                                                                          //clears screen
	        delay_micro(100);                                                                                                       //delay for the command write function
	        if(p==0)                                                                                                                //if the print variable is 0 it'll print
	        {
	            delay_ms(100);                                                                                                      //delay to clear up LCD
	            comWrit(0x86);                                                                                                      //fist line for header
	            delay_micro(100);                                                                                                   //delay for the command write function
	            for(i=0;i<5;i++)                                                                                                    //header for menu
	            {
	                dataWrit(lightMenu[i]);                                                                                         //data write function to write the screen
	            }
	            comWrit(0xC0);                                                                                                      //second row
	            delay_micro(100);                                                                                                   //delay for the command write function
	            for(i=0;i<10;i++)                                                                                                   //lights off string
	            {
	                dataWrit(lightsOff[i]);                                                                                         //data write function to write the screen
	            }
	            comWrit(0x0C);                                                                                                      //command for the LCD
	            delay_ms(500);                                                                                                      //delay for the command write function and to reed
	        }
	        state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;                                                 //clears display and returns to main menu
	        break;                                                                                                                  //END OF THE LIGHTS TURNING OFF STATE



	    case EMERGENCY:                                                                                                             //BEGINING OF THE EMERGENCY STATE
	        if(emerg == 1)                                                                                                          //if the button was pressed
	        {
	            comWrit(0x01);  delay_micro(100);  p=0;                                                                             //clears screen
	            setMotoPWM(0);                                                                                                      //turns off the motor and sets it to zero so you have to reset it
	            if(p==0)                                                                                                            //if the print variable is 0 it'll print
	            {
	                comWrit(0x01);  delay_micro(100);                                                                               //clears the screen
	                delay_ms(100);                                                                                                  //delay to clear up LCD
	                comWrit(0x83);                                                                                                  //Beginning of first line but needs to go to middle to then write to the middle of the screen
	                delay_micro(100);                                                                                               //delay for the command write function
	                for(i=0;i<9;i++)                                                                                                //header for menu
	                {
	                    dataWrit(emergencyMenu[i]);                                                                                 //data write function to write the screen
	                }
	                comWrit(0xC0);                                                                                                  //second row
	                delay_micro(100);                                                                                               //delay for the command write function
	                for(i=0;i<16;i++)                                                                                               //for the emergency prompt
	                {
	                    dataWrit(emerExit[i]);                                                                                      //data write function to write the screen
	                }
	                comWrit(0x90);                                                                                                  //third row
	                delay_micro(100);                                                                                               //delay for the command write function
	                for(i=16;i<25;i++)                                                                                              //rest of the prompt
	                {
	                    dataWrit(emerExit[i]);                                                                                      //data write function to write the screen
	                }
	                comWrit(0xD0);                                                                                                  //fourth row
	                delay_micro(100);                                                                                               //delay for the command write function
	                for(i=25;i<33;i++)                                                                                              //rest of the prompt
	                {
	                    dataWrit(emerExit[i]);                                                                                      //data write function to write the screen
	                }
	                p=1;                                                                                                            //Turns the print variable to 1 so i doesn't keep printing
	            }
	        sleep = 0;              initGreenPWM();                                                                                                         //inits greens PWM
            initBluePWM();                                                                                                          //inits blue PWM
            initRedPWM();                                                                                                           //inits red PWM
            lightOnOff = 1;                                                                                                         //sets light flag
            delay_ms(500);
            offGreenPWM();                                                                                                          //turns off green PWM
            offBluePWM();                                                                                                           //turns off blue PWM
            offRedPWM();                                                                                                            //turns off red PWM
            delay_ms(500);
            lightOnOff = 0;                                                                                                         //sets light flag                                                                                                             //sets sleep variable to zero
	        break;                                                                                                                  //breaks so it doesn't continue
	        }
	        if(emerg == 0)                                                                                                          //if button was pressed again it'll exit
	        {
	            state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;                                             //clears display and returns to main menu
	            break;                                                                                                              //breaks out of the state
	        }
	        break;                                                                                                                  //END OF EMERGENCY STATE



	    case SLEEP:                                                                                                                 //BEGINING OF SLEEP STATE
            delay_ms(250);                                                                                                          //delay to shift screen more slowly
	        if(p==0)                                                                                                                //if the print variable is 0 it'll print
	        {
	            for(i=0;i<10;i++)                                                                                                   //sleep prompt
	            {
	                dataWrit(sleepMenu[i]);                                                                                         //data write function to write the screen
	            }
	            comWrit(0xC5);                                                                                                      //second row
	            delay_micro(100);                                                                                                   //delay for the command write function
	            for(i=10;i<21;i++)                                                                                                  //sleep prompt
	            {
	                dataWrit(sleepMenu[i]);                                                                                         //data write function to write the screen
	            }
	            p=1;                                                                                                                //sets print flag to one so it doesn't print
	        }
            comWrit(0x18);                                                                                                          //shifting addressing
	        getKey();                                                                                                               //gets a key
	        if(new == '*') {state = MAIN_MENU;  comWrit(0x01);  delay_micro(100);  p=0;  sleep = 0;  break;}                        //clears display and returns to main menu
	        comWrit(0x18);                                                                                                          //shifting addressing
	        delay_micro(100);                                                                                                       //delay for the command write function
	        break;                                                                                                                  //END OF SLEEP STATE



	    }                                                                                                                           //END OF THE SWITCH



	    new = '\0';                                                                                                                 //to make sure this char is cleared
	    keyS = 0;                                                                                                                   //to make sure the key pad variable is cleared
	    sleep++;                                                                                                                    //adds one to sleep
	    if(sleep == TIMEOUT)                                                                                                        //if sleep ever equals time out itll go to sleep
	    {
	        state = SLEEP;  comWrit(0x01);  delay_micro(100);  p=0;                                                                 //sets sate to sleep
	    }


	}                                                                                                                               //END OF THE WHILE
}                                                                                                                                   //END OF THE MAIN


char numTOstring(void)                                                                                                              //this function converts the int to a string
{
    char myNum;                                                                                                                     //temporary variable in the function

    if(keyS == 1)       myNum = '#';                                                                                                //if # it makes it a string if the int matches
    if(keyS == 2)       myNum = '9';                                                                                                //if 9 it makes it a string if the int matches
    if(keyS == 4)       myNum = '6';                                                                                                //if 6 it makes it a string if the int matches
    if(keyS == 8)       myNum = '3';                                                                                                //if 3 it makes it a string if the int matches

    if(keyS == 16)      myNum = '0';                                                                                                //if 0 it makes it a string if the int matches
    if(keyS == 32)      myNum = '8';                                                                                                //if 8 it makes it a string if the int matches
    if(keyS == 64)      myNum = '5';                                                                                                //if 5 it makes it a string if the int matches
    if(keyS == 128)     myNum = '2';                                                                                                //if 2 it makes it a string if the int matches

    if(keyS == 256)     myNum = '*';                                                                                                //if * it makes it a string if the int matches
    if(keyS == 512)     myNum = '7';                                                                                                //if 7 it makes it a string if the int matches
    if(keyS == 1024)    myNum = '4';                                                                                                //if 4 it makes it a string if the int matches
    if(keyS == 2048)    myNum = '1';                                                                                                //if 1 it makes it a string if the int matches

    return myNum;                                                                                                                   //returns the string
}
void getKey(void)                                                                                                                   //gets a key function
{
        keyS = readKey();                                                                                                           //gets a value from the matrix keypad as an uint16_t
        new = numTOstring();                                                                                                        //converts int to a string
}
void resetKey()                                                                                                                     //reset key function
{
    for(i=0;i<3;i++)                                                                                                                //for loop to clear the string
          {
                  key[i] = ' ';                                                                                                     //actually sets the string to a space
          }
}
void convertINTmoto(void)                                                                                                           //convert to a integer function
{
    float num=0;                                                                                                                    //num variable to do the conversion
    if(a==1)                                                                                                                        //checks how many keys were entered
    {
        num = key[2]-48;                                                                                                            //converts the string char to a int using ascii logic
    }
    if(a==2)                                                                                                                        //checks how many keys were entered
        {
            num = key[2]-48;                                                                                                        //converts the string char to a int using ascii logic
            num = num + (10 * (key[1]-48));                                                                                         //converts the second place to a number then multiply by 10 to get the tenth place then adds to previous
        }
    if(a==3)                                                                                                                        //checks how many keys were entered
            {
                num = key[2]-48;                                                                                                    //converts the string char to a int using ascii logic
                num = num + (10 * (key[1]-48));                                                                                     //converts the second place to a number then multiply by 10 to get the tenth place then adds to previous
                num = num + (100 * (key[0]-48));                                                                                    //converts to an int then multiples by 100 to get the hundrenths then adds to previous
            }
motoSpd = MOTOSPD * (num/100);                                                                                                      //sets the final integer to a duty cycle by dividing by 100 then multiplying by the period
}
void convertINTred(void)                                                                                                            //red conversion function for brightness
{
    float num=0;                                                                                                                    //num variable to do the conversion
    if(a==1)                                                                                                                        //checks how many keys were entered
    {
        num = key[2]-48;                                                                                                            //converts the string char to a int using ascii logic
    }
    if(a==2)                                                                                                                        //checks how many keys were entered
        {
            num = key[2]-48;                                                                                                        //converts the string char to a int using ascii logic
            num = num + (10 * (key[1]-48));                                                                                         //converts the second place to a number then multiply by 10 to get the tenth place then adds to previous
        }
    if(a==3)                                                                                                                        //checks how many keys were entered
            {
                num = key[2]-48;                                                                                                    //converts the string char to a int using ascii logic
                num = num + (10 * (key[1]-48));                                                                                     //converts the second place to a number then multiply by 10 to get the tenth place then adds to previous
                num = num + (100 * (key[0]-48));                                                                                    //converts to an int then multiples by 100 to get the hundrenths then adds to previous
            }
redBright = MAXBRIGHT * (num/100);                                                                                                  //sets the final integer to a duty cycle by dividing by 100 then multiplying by the period
}
void convertINTblue(void)
{
    float num=0;                                                                                                                    //num variable to do the conversion
    if(a==1)                                                                                                                        //checks how many keys were entered
    {
        num = key[2]-48;                                                                                                            //converts the string char to a int using ascii logic
    }
    if(a==2)                                                                                                                        //checks how many keys were entered
        {
            num = key[2]-48;                                                                                                        //converts the string char to a int using ascii logic
            num = num + (10 * (key[1]-48));                                                                                         //converts the second place to a number then multiply by 10 to get the tenth place then adds to previous
        }
    if(a==3)                                                                                                                        //checks how many keys were entered
            {
                num = key[2]-48;                                                                                                    //converts the string char to a int using ascii logic
                num = num + (10 * (key[1]-48));                                                                                     //converts the second place to a number then multiply by 10 to get the tenth place then adds to previous
                num = num + (100 * (key[0]-48));                                                                                    //converts to an int then multiples by 100 to get the hundrenths then adds to previous
            }
blueBright = MAXBRIGHT * (num/100);                                                                                                 //sets the final integer to a duty cycle by dividing by 100 then multiplying by the period
}
void convertINTgreen(void)                                                                                                          //converts string for green to a int for brightness
{
    float num=0;                                                                                                                    //num variable to do the conversion
    if(a==1)                                                                                                                        //checks how many keys were entered
    {
        num = key[2]-48;                                                                                                            //converts the string char to a int using ascii logic
    }
    if(a==2)                                                                                                                        //checks how many keys were entered
        {
            num = key[2]-48;                                                                                                        //converts the string char to a int using ascii logic
            num = num + (10 * (key[1]-48));                                                                                         //converts the second place to a number then multiply by 10 to get the tenth place then adds to previous
        }
    if(a==3)                                                                                                                        //checks how many keys were entered
            {
                num = key[2]-48;                                                                                                    //converts the string char to a int using ascii logic
                num = num + (10 * (key[1]-48));                                                                                     //converts the second place to a number then multiply by 10 to get the tenth place then adds to previous
                num = num + (100 * (key[0]-48));                                                                                    //converts to an int then multiples by 100 to get the hundrenths then adds to previous
            }
greenBright = MAXBRIGHT * (num/100);                                                                                                //sets the final integer to a duty cycle by dividing by 100 then multiplying by the period
}
void butPres(void)                                                                                                                  //This is our function to see if the button has been pressed
{
    if(!((BUT_PORT->IN & BUT_PIN)))                                                                                                 //This action executes if button is pressed
    {
        delay_ms(15);                                                                                                               //This is our debouncer, its immediately after  the button being pressed
        while(!((BUT_PORT->IN & BUT_PIN))){};                                                                                       //if the button is being held
        if(lightOnOff == 1)                                                                                                         //decides which state it should choose
        {
        state = LIGHT_OFF;                                                                                                          //switches state to light off
        }
        if(lightOnOff == 0)                                                                                                         //decides which state it should choose
               {
               state = LIGHT_ON;                                                                                                    //switches state to light on
               }
    }
}
void BUTTON_IN(void)                                                                                                                //button port interrupt
{
    BUTTON_PORT -> IFG & BUTTON_PIN;                                                                                                //turns flag to 1
    if(!emerg && !(BUTTON_PORT -> IN & BUTTON_PIN))                                                                                 //if the button was pressed and emerg flag isn't 1
    {
        delay_ms(250);                                                                                                              //delay for debounce
        while(!(BUTTON_PORT -> IN & BUTTON_PIN)){}                                                                                  //if button is being held
        p = 0;                                                                                                                      //print variable to zero to print
        emerg = 1;                                                                                                                  //set emerg flag to 1
        state = EMERGENCY;                                                                                                          //changes state
        sleep = 0;                                                                                                                  //sleep variable to zero because something has been done
    }
    if(emerg && !(BUTTON_PORT -> IN & BUTTON_PIN))                                                                                  //if the button was pressed and emerg flag is 1
        {
        delay_ms(250);                                                                                                              //delay for debounce
        while(!(BUTTON_PORT -> IN & BUTTON_PIN)){}                                                                                  //if button is being held
            p = 0;                                                                                                                  //print variable to zero to print
            emerg = 0;                                                                                                              //set emerg flag to 0
            state = EMERGENCY;
            sleep = 0;                                                                                                              //sleep variable to zero because something has been done
        }
    BUTTON_PORT -> IFG &= ~BUTTON_PIN;                                                                                              //turns flag to 0
}
