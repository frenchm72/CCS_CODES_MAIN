/****************************************************************************
  Title:       EGR 226-908 Final Project
  Filename:    Project_Main.c
  Author(s):   Mathew Yerian-French & Malcolm Macdonald
  Date:        11/11/2018 - 12/05/2018
  Instructor:  Professor Scott Zuidema
  Description:
  Note:           There are info descriptions on what is going on in places
               however not every line is commented like the last project.
               There are multiple codes in here that were used from lecture
               but some are commented in our own words Most used code are from
               lecture 226-10 and lab 226-908. i will note where this is at
               best as possible
                  Also there are commented out code that is left in to refer to
               Mistakes for some of the conclusions in the report
*****************************************************************************/

//ALL HEADER FILES USED TO MAKE THIS PROGRAM
#include "msp.h"
#include "LCD.h"
#include "serial.h"
#include "temp.h"
#include "promain.h"
#include "speaker.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

                                                                                                            //THIS GROUP OF VARIBLES ARE FOR THE SERIAL COMUNICATION FOR THE CLOCK
#define BUFFER_SIZE 100
char INPUT_BUFFER[BUFFER_SIZE];
uint8_t storage_location = 0;                                                                               //store location
uint8_t read_location = 0;                                                                                  // read in varible

                                                                                                            //THESE STRING ARRAYS ARE FOR THE DISPLAY TO BE DISPLAYED TO THE LCD LATER
char TempS[TEMPSTR], timeDisp[TIMESTR] = {' ',' ',':',' ',' ',':',' ',' ',},
     Snooz[6] = {'S','N','O','O','Z','E',},
     Blink[2] = {' ',' ',},
     On[3] = {' ','O','N',},
     Off[3] = {'O','F','F',},
     readA[10] = {'R','E','A','D','A','L','A','R','M','\0'},
     readT[9] = {'R','E','A','D','T','I','M','E','\0'},
     terminal[100];                                                                                         //to print invalid or valid TO THE SERIAL MONITOR

                                                                                                            //THIS GROUP INCLUDES FLAGS AND I USED THREW THE PROGRAM TO INTERRUPT ON
int AmPmFlag = 0,
    SerialCom = 0,
    RTCFlag = 0,
    degreeFlag = 1,
    setTimeFlag = 0,
    setAlarmFlag = 0,
    alarmFlag = 0,
    blinkFlag = 0,
    alarmGoFlag = 0,
    snoozFlag = 0,
    MinSecFlag = 0,
    validFlag = 0,
    rAvalidFlag = 0,
    rTvalidFlag = 0,
    sAvalidFlag = 0,
    sTvalidFlag = 0,
    counter = 0,
    i = 0;                                                                                                  //flags to let the main know something happend

                                                                                                            //VARIBLES FOR THE BRIGHTNESS OF THE LEDS
float bright, MaxBright = 7499.00;

                                                                                                            //STATES THIS IS SIMPLY FOR THE DISPLAY
enum states
{
    ALARM_GOING_OFF, ALARM_HOUR, ALARM_MIN,

    NORMAL_CLOCK, TIME_SET_HOUR, TIME_SET_MIN, TIME_SET_SEC,

}state;

                                                                                                            //STUCTURE FOR THE NOW TIME
struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
}now;

                                                                                                            //STRUCTURE FOR THE SET TIME
struct
{
    int8_t sec;
    int8_t min;
    int8_t hour;
}set;

                                                                                                            //STRUCTURE FOR THE ALARM TIME
struct
{
    int8_t min;
    int8_t hour;
}alarm;

                                                                                                            //STRUCT FOR THE SNOOZ TIME
struct
{
    int8_t min;
    int8_t hour;
}snooz;

void main(void)
{
                                                                                                             //SETTING LOCAL VARIBLES FOR THE TEMPATURE AS WELL AS THE INPUT FROM THE SERIAL MONITOR
    float temp;
    char string[BUFFER_SIZE];

                                                                                                             //INITIALY MAKING SOME VARIBLES JUST TO START OFF WITH
    string[0] = '\0';
    INPUT_BUFFER[0]= '\0';                                                                                   //INITIAL SETS THE INPUT BUFFER TO NULL
    alarm.min = 5;
    alarm.hour = 1;

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		                                                         // stop watchdog timer

	                                                                                                         //DISABLES INTURRUPTS
	__disable_irq();

    setupSerial();                                                                                           //sets up serial communication at 9600 8-Bit no parity

                                                                                                             //INITAL FUNCTION THAT INITAL SETS UP INTERFACES USED. SAME AS ALWAYS
	SysTick_Init();
	initPins();
	ADC14init();
	configRTC();

	NVIC -> ISER[0] |= 1 << ADC14_IRQn;                                                                     //adc temp, AND LCD interrupt
    NVIC -> ISER[1] |= 1 <<((PORT1_IRQn)&31);                                                               //enable port 1 interrupts
    NVIC_EnableIRQ(RTC_C_IRQn);                                                                             //THIS IS FOR THE INTURRUPT FOR THE RTC (FOR EVERY ONE SEC)

	__enable_irq();                       	                                                                //Enable all interrupts for MSP432

    delay_ms(100);                                                                                          //so we dont have problems with LCD ONCE IN A WHILE THERE ARE STILL ISSUES BUT THIS STILL HELPS
    initLCD();                                                                                              //INITS THE LCD
    delay_ms(100);                                                                                          //so we dont have problems with LCD

    comWrit(0x0C);  comWrit(0xC0);  delay_ms(1);                                                            //turns off cursor //starts cursor at top left corner/ the first address //delay for the command
    state = NORMAL_CLOCK;                                                                                   //DEFAULT STATE JUST TO INITIALY DISPLAY THE START TIME
    beep();                                                                                                 //TO MAKE SURE SPEAKER WORKS AND DECIDED TO KEEP HERE TO NOTIFY INITIAL START

                                                                                                            //THIS IS A BIG PART IN THE INSTACE OF ONLY CLEARING THE SCREEN HERE AS WELL AS OTHER
                                                                                                            //CERTAIN PLACES BUT THIS MAKES THE DISPLAY NOT FLASH AND MAKE THE TRANSISTION OF THE DIGITS DISPLAYED NICER
    comWrit(0x01);
    delay_ms(100);

	while (1)
	{
/*
 *IF THERE HAS BEEN AN INPUT FROM THE SERIAL MONITOR
 */
	    if(SerialCom)                                                                                       //IF THE SERIAL COMUNICATION GETS A NEWLINE CHARACTER
	    {
	    readInput(string);                                                                                  // Read the input
	    checkStr(string);                                                                                   //checks if the string is valid
	    SerialCom = 0;                                                                                      //RESESTS THE FLAG
	    }

        temp = getTemp(degreeFlag);                                                                         //this is to get the tempature and if the degree flag is 1 it'll be in farhrenheit and if its zero it'll be in celsius
        convertTemp(temp);                                                                                  //this is how you would convert the temp to a string called TempS so we can them print to the screen
        LCDbrightUpdate();                                                                                  //THIS UPDAGTES THE BRIGHTNESS OF THE LCD FROM AN INPUT OF A POTIEOMETER


/*
 * IF THE VALID FLAG IS SET ITLL DECIDE WHAT FUNCTION TO CALL TO THEN CONVERT THE INPUT TO WHAT IS NEEDED
 */
if(validFlag)                                                                                               //WHEN THE COMUNICATION IS CONFERMED TO BE VALID
{
    if(sAvalidFlag)                                                                                         //IF THE SET ALARM HAS BEEN TYPED IN
    {
        convertSerialAlarm(string);                                                                         //CONVERTS IT TO SET THE ALARM
        sAvalidFlag = 0;                                                                                    //CLEARS THE FLAG
    }
    if(sTvalidFlag)
    {
        convertSerialTime(string);
        sTvalidFlag = 0;
    }
    if(rAvalidFlag)
    {
        serialAlarm();
        writeOutput(timeDisp);
                                                                                                            // convertTime();//converts the time to a string
        rAvalidFlag = 0;
    }
    if(rTvalidFlag)
    {
        serialTime();
        writeOutput(timeDisp);
                                                                                                            // convertTime();//converts the time to a string
        rTvalidFlag = 0;
    }
    validFlag = 0;
}
	if(RTCFlag)                                                                                             //alow it to update once every second we can change this later if need be
	{
/*
 * THE SATES ARE SIMPLY FOR THE DISPLAY ONLY. ITLL DECIDE WHICH PLACE TO BLINK ON THE DISPLAY ASWELL AS DISPLAY NORMAL SETTING AND TIME
 */
	    switch(state)
	    {
/*
 *THIS IS THE NORMAL STATE THAT DISPLAYS EVERYTHING TO THE LCD THAT IS NEED TO BE KNOWN
 */
	    case NORMAL_CLOCK:
	        convertTime();                                                                                  //converts the time to a string
            comWrit(0x80);                                                                                  //ADRESS AT FIRST BLANK SPOT
            delay_ms(1);
            displayTime();                                                                                  //DISPLAYS TIME
            if(alarmFlag == 0)
            {
                comWrit(0x8D);                                                                              //ADDRES WHERE THE ALARM IS ON OR OFF
                delay_ms(1);
                for(i=0;i<3;i++)
                {
                    dataWrit(Off[i]);                                                                       //DISPLAYS OFF
                }
            }
            if(alarmFlag == 1)
            {
                comWrit(0x8D);                                                                              //ADDRES WHERE THE ALARM IS ON OR OFF
                delay_ms(1);
                for(i=0;i<3;i++)
                {
                    dataWrit(On[i]);                                                                        //DISPLAYS ON
                }
            }
            if(snoozFlag)
            {
                comWrit(0xC0);
                delay_ms(1);
                for(i=0;i<6;i++)
                {
                    dataWrit(Snooz[i]);                                                                     //DISPLAYS SNOOZ
                }
            }
            if(!snoozFlag)
            {
                comWrit(0xC0);
                delay_ms(1);
                for(i=0;i<6;i++)
                {
                    dataWrit(' ');                                                                          //CLEARS SNOOZ
                }
            }
            comWrit(0xCA);                                                                                  //POSITION ADDRESS TO DISPLAY TEMP
            delay_ms(1);
            displayTemp();                                                                                  //DISPLAYS TEMP
            break;

/*
 * THIS IS TO FLASH THE HOUR PLACE WHEN SETTING THE HOUR IN THE SETING TIME
 * THE BLINK FLAG WILL ALTERNATE ON THE WAY THREW EACH SECOND
 */
	    case TIME_SET_HOUR:
	        convertTimeSet();                                                                               //converts the time to a string
	        comWrit(0x80);                                                                                  //ADRESS AT FIRST BLANK SPOT
	        delay_ms(1);
	        displayTime();                                                                                  //DISPLAYS TIME
	        if(blinkFlag == 1)
	        {
	        comWrit(0x80);                                                                                  //POSITION OF HOURS
	        delay_ms(1);
	        for(i=0;i<2;i++)
	        {
	            dataWrit(Blink[i]);                                                                         //BLINKS THE HOUR PLACE
	        }
	        blinkFlag = 0;
	        }
	        else
	        {
	            blinkFlag = 1;                                                                              //CHANGES BLINK FLAG
	        }
	        if(alarmFlag == 0)
	        {
	            comWrit(0x8D);                                                                              //ADDRES WHERE THE ALARM IS ON OR OFF
	            delay_ms(1);
	            for(i=0;i<3;i++)
	            {
	                dataWrit(Off[i]);                                                                       //DISPLAYS OFF
	            }
	        }
	        if(alarmFlag == 1)
	        {
	            comWrit(0x8D);                                                                              //ADDRES WHERE THE ALARM IS ON OR OFF
	            delay_ms(1);
	            for(i=0;i<3;i++)
	            {
	                dataWrit(On[i]);                                                                        //DISPLAYS ON
	            }
	        }
	        comWrit(0xCA);                                                                                  //POSITION ADDRESS TO DISPLAY TEMP
	        delay_ms(1);
	        displayTemp();                                                                                  //DISPLAYS TEMP
	        break;

/*
 * THIS IS TO FLASH THE MIN PLACE WHEN SETTING THE MIN IN THE SETING TIME
 * THE BLINK FLAG WILL ALTERNATE ON THE WAY THREW EACH SECOND
 */
        case TIME_SET_MIN:
            convertTimeSet();                                                                               //converts the time to a string
            comWrit(0x80);                                                                                  //ADRESS AT FIRST BLANK SPOT
            delay_ms(1);
            displayTime();                                                                                  //DISPLAYS TIME
            if(blinkFlag == 1)
            {
            comWrit(0x83);                                                                                  //POSIOTION OF MINUTES
            delay_ms(1);
            for(i=0;i<2;i++)
            {
                dataWrit(Blink[i]);                                                                         //BLINKS MIN PLACE
            }
            blinkFlag = 0;
            }
            else
            {
                blinkFlag = 1;
            }
            if(alarmFlag == 0)
            {
                comWrit(0x8D);                                                                              //POSITION WHERE ON OR OFF IS WRITTEN TO
                delay_ms(1);
                for(i=0;i<3;i++)
                {
                    dataWrit(Off[i]);                                                                       //DISPLAYS OFF
                }
            }
            if(alarmFlag == 1)
            {
                comWrit(0x8D);                                                                              //POSITION WHERE ON OR OFF IS WRITTEN TO
                delay_ms(1);
                for(i=0;i<3;i++)
                {
                    dataWrit(On[i]);                                                                        //DISPLAYS ON
                }
            }
            comWrit(0xCA);                                                                                  //POSITION ADDRESS TO DISPLAY TEMP
            delay_ms(1);
            displayTemp();                                                                                  //DISPLAYS TEMP
            break;


/*
 * THIS IS TO FLASH THE SEC PLACE WHEN SETTING THE SEC IN THE SETING TIME
 * THE BLINK FLAG WILL ALTERNATE ON THE WAY THREW EACH SECOND
 */
        case TIME_SET_SEC:
            convertTimeSet();                                                                               //converts the time to a string
            comWrit(0x80);                                                                                  //ADRESS AT FIRST BLANK SPOT
            delay_ms(1);
            displayTime();                                                                                  //DISPLAYS TIME
            if(blinkFlag == 1)
            {
            comWrit(0x86);                                                                                  //ADDRESS TO SECONDS PLACE
            delay_ms(1);
            for(i=0;i<2;i++)
            {
                dataWrit(Blink[i]);                                                                         //BLINKS SECONS PLACE
            }
            blinkFlag = 0;
            }
            else
            {
                blinkFlag = 1;
            }
            if(alarmFlag == 0)
            {
                comWrit(0x8D);                                                                              //ADDRES WHERE THE ALARM IS ON OR OFF
                delay_ms(1);
                for(i=0;i<3;i++)
                {
                    dataWrit(Off[i]);                                                                       //DISPLAYS OFF
                }
            }
            if(alarmFlag == 1)
            {
                comWrit(0x8D);                                                                              //ADDRES WHERE THE ALARM IS ON OR OFF
                delay_ms(1);
                for(i=0;i<3;i++)
                {
                    dataWrit(On[i]);                                                                        //DISPLAYS ON
                }
            }
            comWrit(0xCA);                                                                                  //POSITION ADDRESS TO DISPLAY TEMP
            delay_ms(1);
            displayTemp();                                                                                  //DISPLAYS TEMP
            break;

/*
 * THIS IS TO FLASH THE HOURS PLACE WHEN SETTING THE HOURS IN THE ALARM
 * THE BLINK FLAG WILL ALTERNATE ON THE WAY THREW EACH SECOND
 */
        case ALARM_HOUR:
            convertAlarmSet();                                                                              //converts the time to a string
            comWrit(0x80);                                                                                  //ADRESS AT FIRST BLANK SPOT
            delay_ms(1);
            displayTime();                                                                                  //DISPLAYS TIME
            if(blinkFlag == 1)
            {
                comWrit(0x80);                                                                              //ADRESS AT FIRST BLANK SPOT
                delay_ms(1);
                for(i=0;i<2;i++)
                {
                    dataWrit(Blink[i]);                                                                     //BLINKS HOURS PLACE
                }
                blinkFlag = 0;
            }
            else
            {
                blinkFlag = 1;
            }
            if(alarmFlag == 0)
            {
                comWrit(0x8D);                                                                              //ADDRES WHERE THE ALARM IS ON OR OFF
                delay_ms(1);
                for(i=0;i<3;i++)
                {
                    dataWrit(Off[i]);                                                                       //DISPLAYS OFF
                }
            }
            if(alarmFlag == 1)
            {
                comWrit(0x8D);                                                                              //ADDRES WHERE THE ALARM IS ON OR OFF
                delay_ms(1);
                for(i=0;i<3;i++)
                {
                    dataWrit(On[i]);                                                                        //DISPLAUYS ON
                }
            }
            comWrit(0xCA);                                                                                  //POSITION ADDRESS TO DISPLAY TEMP
            delay_ms(1);
            displayTemp();                                                                                  //DISPLAYS TEMP
            break;


/*
 * THIS IS TO FLASH THE MINS PLACE WHEN SETTING THE MINUTES IN THE ALARM
 * THE BLINK FLAG WILL ALTERNATE ON THE WAY THREW EACH SECOND
 */
        case ALARM_MIN:
            convertAlarmSet();                                                                              //converts the time to a string
            comWrit(0x80);                                                                                  //ADRESS AT FIRST BLANK SPOT
            delay_ms(1);
            displayTime();                                                                                  //DISPLAYS TIME
            if(blinkFlag == 1)
            {
                comWrit(0x83);                                                                              //POSITION OF MINUTES PLACE
                delay_ms(1);
                for(i=0;i<2;i++)
                {
                    dataWrit(Blink[i]);                                                                     //BLINKS MINUTES PLACE
                }
                blinkFlag = 0;
            }
            else
            {
                blinkFlag = 1;
            }
            if(alarmFlag == 0)
            {
                comWrit(0x8D);                                                                              //ADDRES WHERE THE ALARM IS ON OR OFF
                delay_ms(1);
                for(i=0;i<3;i++)
                {
                    dataWrit(Off[i]);                                                                       //DISPLAYS OFF
                }
            }
            if(alarmFlag == 1)
            {
                comWrit(0x8D);                                                                              //ADDRES WHERE THE ALARM IS ON OR OFF
                delay_ms(1);
                for(i=0;i<3;i++)
                {
                    dataWrit(On[i]);                                                                        //DISPLAYS ON
                }
            }
            comWrit(0xCA);                                                                                  //POSITION ADDRESS TO DISPLAY TEMP
            delay_ms(1);
            displayTemp();                                                                                  //DISPLAYS TEMP
            break;
/*
 * THIS IS TO FLASH THE SCREEN WHEN THE ALARM IS GOING OFF
 * THE BLINK FLAG WILL ALTERNATE ON THE WAY THREW EACH SECOND
 */
        case ALARM_GOING_OFF:
            beep();                                                                                         //BEEP THE ALARM
            if(blinkFlag)
            {
                comWrit(0x01);                                                                              //CLEARS SCREEN TO BLINK
                delay_ms(10);
                blinkFlag = 0;                                                                              //RESETS FLAG
                break;
            }
            if(blinkFlag==0)
            {
            convertTime();                                                                                  //converts the time to a string
            comWrit(0x80);                                                                                  //ADRESS AT FIRST BLANK SPOT
            delay_ms(1);
            displayTime();                                                                                  //DISPLAYS TIME
            if(alarmFlag == 0)
            {
            comWrit(0x8D);                                                                                  //ADDRES WHERE THE ALARM IS ON OR OFF
            delay_ms(1);
            for(i=0;i<3;i++)
            {
                dataWrit(Off[i]);                                                                           //DISPLAYS OFF
            }
            }
            if(alarmFlag == 1)
                        {
                        comWrit(0x8D);                                                                      //POSITON TO DISPLAY ON OR OFF
                        delay_ms(1);
                        for(i=0;i<3;i++)
                        {
                            dataWrit(On[i]);                                                                //DISPLAYS ON
                        }
                        }
            comWrit(0xCA);                                                                                  //POSITION ADDRESS TO DISPLAY TEMP
            delay_ms(1);
            displayTemp();                                                                                  //DISPLAYS TEMP
            blinkFlag = 1;
            break;
            }
            break;


                                                                                                              //default for normal clock in case it gets lost
        default:
            state = NORMAL_CLOCK;
            break;
	    }                                                                                                     //end of switch

/*
 *      these two if statements should be the same to basicly increase brightness for the wake
 *      up leds one is for the approche sleep and the other is for the approche on the alarm
 *      THIS WILL COMBAT THE EFFECT OF SAY THE TIME BEING AT 12:01 AM ITLL START TO INCREASE BRIGHTNESS AT 11:56 PM TO 1% EVERY 3 SECONDS
 */
	                                                                                                          //if((alarmFlag == 1) && ((alarm.min - now.min)<=5) && ((alarm.hour == now.hour)||((alarm.min < 5)&&((alarm.hour-1) == now.hour))) && ((alarm.min - now.min)>0))
	    if((alarmGoFlag == 0)&&(alarmFlag == 1) && ((((alarm.hour *60)+alarm.min)) <= ((now.hour*60)+now.min+5)) && (((alarm.hour*60)+alarm.min >= 5)))
	    {
	        counter++;
	        if(counter==3)
	        {
	            counter = 0;
	            bright = bright + (MaxBright * .01);
	            TIMER_A0 -> CCR[WAKE_INST] = bright;
	                                                                                                            // <--- set this equal to duty cycle add 1% brigntness to led for 5 mins before alarm for every 3 seconds
	        }
	    }
	    if((alarmGoFlag == 0)&&(alarmFlag == 1) && (((alarm.hour*60)+alarm.min < 5))&&((alarm.min + ((23*60)+55))<= ((now.hour*60)+now.min)))
	    {
	        counter++;
	                    if(counter==3)
	                    {
	                        counter = 0;
	                        bright = bright + (MaxBright * .01);
	                        TIMER_A0 -> CCR[WAKE_INST] = bright;
	                                                                                                            // <--- set this equal to duty cycle add 1% brigntness to led for 5 mins before alarm for every 3 seconds
	                    }
	    }
	    if((alarmGoFlag == 0)&&(alarmFlag == 1) && ((alarm.hour*60)+alarm.min < 5)&&(alarm.hour==now.hour)&&(alarm.min>=now.min))
        {
            counter++;
                        if(counter==3)
                        {
                            counter = 0;
                            bright = bright + (MaxBright * .01);
                            TIMER_A0 -> CCR[WAKE_INST] = bright;
                                                                                                                // <--- set this equal to duty cycle add 1% brigntness to led for 5 mins before alarm for every 3 seconds
                        }
        }
	                                                                                                            // if((snoozFlag == 1) && ((snooz.min - now.min)<=5) && ((snooz.hour == now.hour)||((snooz.min < 5)&&((snooz.hour-1) == now.hour))) && ((snooz.min - now.min)>0))
	    if((alarmGoFlag == 1)&&(snoozFlag == 1) && ((((snooz.hour *60)+snooz.min)) <= ((now.hour*60)+now.min+5)) && (((snooz.hour*60)+snooz.min >= 5)))
        {
            counter++;
            if(counter==3)
            {
                counter = 0;
                bright = bright + (MaxBright * .01);
                TIMER_A0 -> CCR[WAKE_INST] = bright;
            }
        }
        if((alarmGoFlag == 1)&&(snoozFlag == 1) && (((snooz.hour*60)+snooz.min < 5))&&((snooz.min + ((23*60)+55))<= ((now.hour*60)+now.min)))
        {
            counter++;
                        if(counter==3)
                        {
                            counter = 0;
                            bright = bright + (MaxBright * .01);
                            TIMER_A0 -> CCR[WAKE_INST] = bright;
                                                                                                                // <--- set this equal to duty cycle add 1% brigntness to led for 5 mins before alarm for every 3 seconds
                        }
        }
        if((alarmGoFlag == 1)&&(snoozFlag == 1) && ((snooz.hour*60)+snooz.min < 5)&&(snooz.hour==now.hour)&&(snooz.min>=now.min))
        {
            counter++;
                        if(counter==3)
                        {
                            counter = 0;
                            bright = bright + (MaxBright * .01);
                            TIMER_A0 -> CCR[WAKE_INST] = bright;
                                                                                                                // <--- set this equal to duty cycle add 1% brigntness to led for 5 mins before alarm for every 3 seconds
                        }
        }

/*
 * these set the alarm off as when it is in sleep or the alarm setting
 * THIS IS THE ALARM LOGIC TO MAKE THE ALARM GO OFF WHEN IT IS TRUE TO ALOW THE SPEAKER TO MAKE NOISE AND SET APPROPRIOT FLAGS
 */
        if((alarm.hour == now.hour) && (alarm.min == now.sec) && (MinSecFlag == 1))
               {
                   RTC_C -> TIM0 = now.sec<<8;
                   state = ALARM_GOING_OFF;
                   alarmGoFlag = 1;
                   MinSecFlag = 0;
               }
	    if((alarm.hour == now.hour) && (alarm.min == now.min) && (alarmFlag == 1) && (0 == now.sec))
	    {
	        state = ALARM_GOING_OFF;
	        alarmGoFlag = 1;
	    }
	    if((snooz.hour == now.hour) && (snooz.min == now.min) && (snoozFlag == 1) && (0 == now.sec) && (alarmFlag == 1))
	    {
            state = ALARM_GOING_OFF;
	        snoozFlag = 0;
	    }


/*
 * this basically makes the value of the min to 59 so when seconds == 59 itll increase the hours INCREASEING MINUTES BY SECONDS
 * THIS IS LOGIC INCASE THE HOURS NEEDS TO BE INCREASED
 */
	    if((now.min!=59)&&(MinSecFlag == 1))
	        {
	        RTC_C -> TIM0 = 59<<8;
	        }
	    RTCFlag = 0;                                                                                                    //do not move this is to alow the LCD refresh and increment the RTC
	        }                                                                                                           //End of if second statement
	}                                                                                                                   //end of while loop
}                                                                                                                       //end of main

/*
 * SIMPLE FUNCTION THAT DISPLAYS THE TIME TO THE LCD
 */
void displayTime(void)
{
    for(i=0;i<TIMESTR;i++)
    {
        dataWrit(timeDisp[i]);
    }
}

/*
 * SIMPLE FUNCTION TO PRINT THE TEMP
 */
void displayTemp(void)
{
    for(i=0;i<TEMPSTR;i++)
    {
        dataWrit(TempS[i]);
    }
}

/*
 * THIS IS FOR THE TIME THAT WILL BE DISPLAY ON THE LCD
 * THIS FUNCTION CONVERTS THE MILITARY TIME THATS USED BY THE SOFTWARE TO A STRING TO DISPLAY
 * THIS LOGIC IS USED THREW THE PROGRAM
 * ITS EXACLY THE SAME IN EXCEPTION OF WHAT VARIBLES THERE ARE
 * ITS DISTINGUISHES FROM AM AND PM ASWELL AND COPY TO THE STRING TO THEN BE DISPLAYED
 * THE SAME STRING IS USED THREW THESE TYPES OF FUNCTIONS THAT CONVERTS THE NUMBER OT THE STRING
 */
void convertTime (void)
{

if(now.hour == 0)
{
    timeDisp[0] = '1';
    timeDisp[1] = '2';
    AmPmFlag = 0;
}
if((now.hour == 12))
{
    timeDisp[0] = ((now.hour/10)%10)+48;
    timeDisp[1] = (now.hour%10)+48;
    AmPmFlag = 1;
}
if((now.hour < 10) && (now.hour != 0))
{
   timeDisp[0] = ' ';
   timeDisp[1] = (now.hour+48);
   AmPmFlag = 0;
}
if((now.hour >= 10) && (now.hour < 12))
{
    timeDisp[0] = ((now.hour/10)%10)+48;
    timeDisp[1] = (now.hour%10)+48;
    AmPmFlag = 0;
}
if((now.hour > 21)&&(now.hour < 24))
{
    timeDisp[0] = (((now.hour-12)/10)%10)+48;
    timeDisp[1] = ((now.hour-12)%10)+48;
    AmPmFlag = 1;
}
if((now.hour > 12)&&(now.hour<22))
{
    timeDisp[0] = ' ';
    timeDisp[1] = ((now.hour-12)%10)+48;
    AmPmFlag = 1;
}
if(MinSecFlag == 0)
{
if(now.min < 10)
{
    timeDisp[3] = '0';
    timeDisp[4] = (now.min+48);
}
if(now.min >= 10)
{
    timeDisp[3] = ((now.min/10)%10)+48;
    timeDisp[4] = (now.min%10)+48;
}
if(now.sec < 10)
{
    timeDisp[6] = '0';
    timeDisp[7] = (now.sec+48);
}
if(now.sec >= 10)
{
    timeDisp[6] = ((now.sec/10)%10)+48;
    timeDisp[7] = (now.sec%10)+48;
}
}
if(MinSecFlag == 1)
{
if(now.sec < 10)
{
    timeDisp[3] = '0';
    timeDisp[4] = (now.sec+48);
}
if(now.sec >= 10)
{
    timeDisp[3] = ((now.sec/10)%10)+48;
    timeDisp[4] = (now.sec%10)+48;
}
timeDisp[6] = '0';
timeDisp[7] = '0';
}
if(AmPmFlag == 0)                                                                                                                       //COPYS AM IF THE FLAG IS NOT SET
{
    timeDisp[8] = ' ';
    timeDisp[9] = 'A';
    timeDisp[10] = 'M';
}
if(AmPmFlag == 1)                                                                                                                           //COPYS PM IF THE FLAG IS SET
{
    timeDisp[8] = ' ';
    timeDisp[9] = 'P';
    timeDisp[10] = 'M';
}

}

/*
 * THIS IS THE RTC HANDLER
 * THE RTC INTERRUPTS ON EVERY SECOND THAT HAS PASSED AND SETS THE RIGHT FLAG
 * THIS ALSO SETS THE VALUE IN THE RTC IN TO THE NOW STRUCTURE
 */
void RTC_C_IRQHandler(void)
{
    now.sec = RTC_C -> TIM0>>0 & 0x00FF;
    now.min = RTC_C -> TIM0>>8 & 0x00FF;
    now.hour = RTC_C -> TIM1>>0 & 0x00FF;

    RTCFlag = 1;
    RTC_C -> CTL0 = 0xA510;
}

/*
 * THIS CONVERTS THE RAW VALUE OF THE TEMPATURE VALURE INTO A STRING TO THEN BE DISPLAYED TO THE SCREEN
 * ITLL ALSO PLACE THE f OR c FOR WHICH DEGREE THAT IS CHOOSEN
 */
void convertTemp(float TEMP)                                                                                                //converts the number to a string
{
    int t;
    t = TEMP * 100;

    TempS[0] = (t/1000)+48;
    TempS[1] = ((t/100)%10)+48;
    TempS[2] = '.';
    TempS[3] = ((t/10)%10)+48;
    //TempS[4] = (t%10)+48;
    TempS[4] = 0b11011111;                                                                                                   //for the degree symbol
    if(degreeFlag == 1)
    {
        TempS[5] = 'F';
    }
    else
    {
        TempS[5] = 'C';
    }
}

 /*
  * THIS IS THE PORT 1 INTERRUPT HANDLER
  * THIS WILL DECIDE WHICH BUTTON IS HIT ON A GIVEN FLAG THEN SET IT TO A NEW FLAG
  * ITLL INCREMENT HOURS MIN FOR BOTH ALARM AND SETTING THE TIME AND IT ALSO DOES SECONDS FOR SETING THE TIME
  * ITLL DECREMENT HOURS MIN FOR BOTH ALARM AND SETTING THE TIME AND IT ALSO DOES SECONDS FOR SETING THE TIME
  * ITLL DECIDED WHAT SETTINGS IT IS IN WHEATHER ITS SETTING ALARM TIME OR TO SNOOZ OR TURN THE ALARM ON OR OFF
  */
void BUTTON_IN(void)                                                                                                                //button port interrupt
{
    ONOFFUP_PORT -> IFG & ONOFFUP_PIN;                                                                                                  //THIS IS IF THE FLAG IS SET
    SDOWN_PORT -> IFG & SDOWN_PIN;
    SET_PORT -> IFG & SET_PIN;
    SETALARM_PORT -> IFG & SETALARM_PIN;
    MINSEC_PORT -> IFG & MINSEC_PIN;
    BUTTON_PORT -> IFG & BUTTON_PIN;

    if(!(MINSEC_PORT -> IN & MINSEC_PIN) && (MinSecFlag == 0))                                                                          //TO INCRESE BY MINUTES BY SECONDS
    {
        delay_ms(DEBOUN);
        RTC_C -> TIM0 = now.min;
        while(!(MINSEC_PORT -> IN & MINSEC_PIN)){}
        RTC_C -> TIM0 = now.min;
        MinSecFlag = 1;
    }
    if(!(MINSEC_PORT -> IN & MINSEC_PIN) && (MinSecFlag == 1))                                                                              //TO INCREASE BY SECONDS BY SECONDS
        {
            delay_ms(DEBOUN);                                                                                                              //delay for debounce
            while(!(MINSEC_PORT -> IN & MINSEC_PIN)){}
            RTC_C -> TIM0 = now.sec<<8;
            MinSecFlag = 0;
        }

    /*
     * THIS PART IS FOR SETTING THE TIME ASLONG AS ITS NOT IN SET ALARM
     */
   if((setTimeFlag == 1) && (setAlarmFlag == 0))
        {
       if(!(SDOWN_PORT -> IN & SDOWN_PIN) && (state == TIME_SET_HOUR))                                                                      //THIS WILL DECRESS THE HOURS UNLESS IT IS 0 ITLL GO TO 23
       {
           delay_ms(DEBOUN);                                                                                                              //delay for debounce
           while(!(SDOWN_PORT -> IN & SDOWN_PIN)){}                                                                                  //if button is being held
           if(set.hour >= 0)
           set.hour--;
           if(set.hour < 0)
           {
             set.hour = 23;
           }
           MinSecFlag = 0;

       }
       if(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN) && (state == TIME_SET_HOUR))                                                              //INCREASES THE VALUE OF HOURS INLESS IT IS 24 THEN ITLL GO TO 00
       {
           delay_ms(DEBOUN);                                                                                                              //delay for debounce
           while(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN)){}                                                                                  //if button is being held
           if(set.hour<24)
           set.hour++;
           if(set.hour == 24)
                      {
                          set.hour = 0;
                      }
           MinSecFlag = 0;
       }
       if(!(SDOWN_PORT -> IN & SDOWN_PIN) && (state == TIME_SET_MIN))                                                           //THIS IS TO DECREMENT THE MINUTES AND DECIDE WHAT TO DO AFTER THAT IF CERTAIN VALUES
       {
           delay_ms(DEBOUN);                                                                                                              //delay for debounce
           while(!(SDOWN_PORT -> IN & SDOWN_PIN)){}                                                                                  //if button is being held
           if(set.min >= 0)
           set.min--;
           if(set.min < 0)
           {
               set.hour--;
               if(set.hour <= 0)
               {
                   set.hour = 23;
               }
               set.min = 59;
           }
           MinSecFlag = 0;
       }
       if(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN) && (state == TIME_SET_MIN))                                                                   //THIS IS FOR THE MINUTS PLACE TO INCREMENT
       {
           delay_ms(DEBOUN);                                                                                                              //delay for debounce
           while(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN)){}                                                                                  //if button is being held
           if(set.min <= 60)
           set.min++;
           if(set.min >= 60)
           {
               set.hour++;
               if(set.hour == 24)
               {
                   set.hour = 0;
               }
               set.min = 0;
           }
           MinSecFlag = 0;
       }
       if(!(SDOWN_PORT -> IN & SDOWN_PIN) && (state == TIME_SET_SEC))                                                           //THIS OGIC WILL DECIDE TO DECREMENT AND IF A CERTAIN VALUE IT WILL DECREMENT MORE
       {
           delay_ms(DEBOUN);                                                                                                              //delay for debounce
           while(!(SDOWN_PORT -> IN & SDOWN_PIN)){}                                                                                  //if button is being held
           if(set.sec >= 0)
                      set.sec--;
                      if(set.sec < 0)
                      {
                          set.min--;
                          if(set.min < 0)
                          {
                              set.min = 59;
                              set.hour--;
                              if(set.hour <= 0)
                              {
                                  set.hour = 23;
                              }
                          }
                          set.sec = 59;
                      }
                      MinSecFlag = 0;
       }
       if(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN) && (state == TIME_SET_SEC))                                                               //THIS LOGIC WILL INCRESS THE SECONDS AND ADJUST THE OTHER PLACES IF THE CERTAIN VALUES
       {
           delay_ms(DEBOUN);                                                                                                              //delay for debounce
           while(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN)){}                                                                                  //if button is being held
           if(set.sec <= 60)
                      set.sec++;
                      if(set.sec >= 60)
                      {
                          set.min++;
                          if(set.min >= 60)
                          {
                              set.min = 0;
                              set.hour++;
                              if(set.hour == 24)
                              {
                                  set.hour = 0;
                              }
                          }
                          set.sec = 0;
                      }
                      MinSecFlag = 0;
       }

        if(!(SET_PORT -> IN & SET_PIN) && (state == TIME_SET_HOUR))                                                                     //THIS IS TO GO THE THE MIUTES PLACE FROM THE HOURS PLACE
                {
                delay_ms(DEBOUN);                                                                                                              //delay for debounce
                while(!(SET_PORT -> IN & SET_PIN)){}                                                                                  //if button is being held
                    state = TIME_SET_MIN;
                }
        if(!(SET_PORT -> IN & SET_PIN) && (state == TIME_SET_MIN))                                                                      //THIS IS TO GO TO THE SECONDS PLACE FROM THE MINUTES PLACE
                {
                delay_ms(DEBOUN);                                                                                                              //delay for debounce
                while(!(SET_PORT -> IN & SET_PIN)){}                                                                                  //if button is being held
                    state = TIME_SET_SEC;
                }
        if(!(SET_PORT -> IN & SET_PIN) && (state == TIME_SET_SEC))                                                                      //THIS IS TO GO BACK TO THE NORMAL CLOCK COMING FROM THE SECONDS PLACE
                {
                delay_ms(DEBOUN);                                                                                                              //delay for debounce
                while(!(SET_PORT -> IN & SET_PIN)){}                                                                                  //if button is being held
                    state = NORMAL_CLOCK;
                    setRTC();
                    setTimeFlag = 0;
                    now.hour = set.hour;
                    now.min = set.min;
                    now.sec = set.sec;
                }
        }


   /*
    * THIS PART IS THE NORMAL FUNCTION OF THE BUTTON
    */
    if((setTimeFlag == 0) && (setAlarmFlag == 0))
    {
    if(!(SET_PORT -> IN & SET_PIN))                                                                                                 //THIS IS FOR THE SET TIME SO IT COPYS THE NOW TIME OVER THEN CHANGES THE NESSISARY FLAH
            {
            delay_ms(DEBOUN);                                                                                                              //delay for debounce
            while(!(SET_PORT -> IN & SET_PIN)){}                                                                                  //if button is being held
                setTimeFlag = 1;
                state = TIME_SET_HOUR;
                set.hour = now.hour;
                set.min = now.min;
                set.sec = now.sec;
                MinSecFlag = 0;
            }
    if(!(SETALARM_PORT -> IN & SETALARM_PIN))                                                                                                   //IF THE SET ALARM BUTTON IS PRESSED CHANGE THE FLAG
                {
                delay_ms(DEBOUN);                                                                                                              //delay for debounce
                while(!(SETALARM_PORT -> IN & SETALARM_PIN)){}                                                                                  //if button is being held
                    setAlarmFlag = 1;
                    state = ALARM_HOUR;
                    MinSecFlag = 0;
                }
    if((alarmFlag == 0) && (!(ONOFFUP_PORT -> IN & ONOFFUP_PIN)))                                                                           //IF THE ALARM IS OFF TURN IT ON
    {
        delay_ms(DEBOUN);                                                                                                              //delay for debounce
        while(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN)){}                                                                                  //if button is being held
        alarmFlag = 1;
        MinSecFlag = 0;
        //bright = MAXBRIGHT * .05;
    }
    if((alarmFlag == 1) && (!(ONOFFUP_PORT -> IN & ONOFFUP_PIN)))                                                                           //IF THE ALARM IS ON TURN IT OFF
        {
            delay_ms(DEBOUN);                                                                                                              //delay for debounce
            while(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN)){}                                                                                  //if button is being held
            alarmFlag = 0;
            counter = 0;
            alarmGoFlag = 0;
            state = NORMAL_CLOCK;
            bright = 0;                                                                                                                     //*********************************************************************set the led to 0 brightness
            snoozFlag = 0;
            TIMER_A0 -> CCR[WAKE_INST] = bright;
            MinSecFlag = 0;
        }
    }

/*
 * THIS PART IS FOR THE SETTING THE ALARM AND DOSE THE MATH IN THE BACKGROUND
 */
    if((setAlarmFlag == 1))
           {
          if(!(SDOWN_PORT -> IN & SDOWN_PIN) && (state == ALARM_HOUR))                                                                  //THIS LOGIC WILL DECIDE WHAT TO DECRESS IN THE HOUR POSITION
          {
              delay_ms(DEBOUN);                                                                                                              //delay for debounce
              while(!(SDOWN_PORT -> IN & SDOWN_PIN)){}                                                                                  //if button is being held
              if(alarm.hour >= 0)
                  alarm.hour--;
              if(alarm.hour < 0)
              {
                  alarm.hour = 23;
              }
              MinSecFlag = 0;
          }
          if(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN) && (state == ALARM_HOUR))                                                              //THIS LOGIC WILL DECIDE WHAT TO INCRESSE FOR THE HOUR POSITION
          {
              delay_ms(25);                                                                                                              //delay for debounce
              while(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN)){}                                                                                  //if button is being held
              if(alarm.hour<24)
                  alarm.hour++;
              if(alarm.hour == 24)
                         {
                  alarm.hour = 0;
                         }
              MinSecFlag = 0;
          }
          if(!(SDOWN_PORT -> IN & SDOWN_PIN) && (state == ALARM_MIN))                                                                   //THIS LOGIC WILL DECID EXACLY WHAT TO DECREMENT
          {
              delay_ms(25);                                                                                                              //delay for debounce
              while(!(SDOWN_PORT -> IN & SDOWN_PIN)){}                                                                                  //if button is being held
              if(alarm.min >= 0)
                  alarm.min--;
              if(alarm.min < 0)
              {
                  alarm.hour--;
                  if(alarm.hour < 0)
                  {
                      alarm.hour = 23;
                  }
                  alarm.min = 59;
              }
              MinSecFlag = 0;
          }
          if(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN) && (state == ALARM_MIN))                                                                       //TO DECIDE IF YOU CAN ADD 1 TO THE MINS PLACE FOR THE ALARM
          {
              delay_ms(DEBOUN);                                                                                                              //delay for debounce
              while(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN)){}                                                                                  //if button is being held
              if(alarm.min < 60)
                  alarm.min++;
              if(alarm.min == 60)
              {
                  alarm.hour++;
                  if(alarm.hour == 24)
                  {
                      alarm.hour = 0;
                  }
                  alarm.min = 0;
              }
              MinSecFlag = 0;
          }
        /*  if(!(SDOWN_PORT -> IN & SDOWN_PIN) && (state == ALARM_SEC))
          {
              delay_ms(DEBOUN);                                                                                                              //delay for debounce
              while(!(SDOWN_PORT -> IN & SDOWN_PIN)){}                                                                                  //if button is being held
              if(alarm.sec >= 0)
                  alarm.sec--;
                         if(alarm.sec < 0)
                         {
                             alarm.min--;
                             if(alarm.min < 0)
                             {
                                 alarm.min = 59;
                                 alarm.hour--;
                                 if(alarm.hour <= 0)
                                 {
                                     alarm.hour = 24;
                                 }
                             }
                             alarm.sec = 59;
                         }
          }
          if(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN) && (state == ALARM_SEC))
          {
              delay_ms(DEBOUN);                                                                                                              //delay for debounce
              while(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN)){}                                                                                  //if button is being held
              if(alarm.sec <= 60)
                  alarm.sec++;
                         if(alarm.sec >= 60)
                         {
                             alarm.min++;
                             if(alarm.min >= 60)
                             {
                                 alarm.min = 0;
                                 alarm.hour++;
                                 if(alarm.hour == 25)
                                 {
                                     alarm.hour = 1;
                                 }
                             }
                             alarm.sec = 0;
                         }
          }*/

           if(!(SETALARM_PORT -> IN & SETALARM_PIN) && (state == ALARM_HOUR))                                                                       //GO TTO THE MINUTES PLACE TO SET IF THE BUTTON HAS BEEN HIT
                   {
                   delay_ms(DEBOUN);                                                                                                              //delay for debounce
                   while(!(SETALARM_PORT -> IN & SETALARM_PIN)){}                                                                                  //if button is being held
                       state = ALARM_MIN;
                   }
           if(!(SETALARM_PORT -> IN & SETALARM_PIN) && (state == ALARM_MIN))                                                                    //TRANSITION TO NORMAL CLOCK AFTER THE BUTTON HAS BEEN HIT
                   {
                   delay_ms(DEBOUN);                                                                                                              //delay for debounce
                   while(!(SETALARM_PORT -> IN & SETALARM_PIN)){}                                                                                  //if button is being held
                       //state = ALARM_SEC;
                   state = NORMAL_CLOCK;
                   setAlarmFlag = 0;
                   convertTime();
                   timeDisp[5] = ':';
                   MinSecFlag = 0;
                   }
          /* if(!(SETALARM_PORT -> IN & SETALARM_PIN) && (state == ALARM_SEC))
                   {
                   delay_ms(DEBOUN);                                                                                                              //delay for debounce
                   while(!(SETALARM_PORT -> IN & SETALARM_PIN)){}                                                                                  //if button is being held
                       state = NORMAL_CLOCK;
                       setAlarmFlag = 0;
                       convertTime();
                       timeDisp[5] = ':';
                   }*/
           }
    if((alarmGoFlag == 1) && (!(ONOFFUP_PORT -> IN & ONOFFUP_PIN)))                                                                         //TURNS OFF THE ALARM WHEN IT IS GOING OFF AND CHANGES THE ALARM TO ZERO TO MAKE IT TURN OFF
            {
                delay_ms(DEBOUN);                                                                                                              //delay for debounce
                while(!(ONOFFUP_PORT -> IN & ONOFFUP_PIN)){}                                                                                  //if button is being held
                alarmFlag = 0;
                counter = 0;
                alarmGoFlag = 0;
                bright = 0;                                                                                                                         //***set the led to 0 brightness
                snoozFlag = 0;                                                                                                                      //CLEARS SNOOZ FLAG
                state = NORMAL_CLOCK;                                                                                                               //SETS TO NORMAL STATE
                TIMER_A0 -> CCR[WAKE_INST] = bright;                                                                                                //SHUTS OFF LEDS
                MinSecFlag = 0;                                                                                                                     //MAKES MIN SEC SET TO CONTINUE IN SECONDS
            }
    if((alarmGoFlag == 1) && !(SDOWN_PORT -> IN & SDOWN_PIN) && (snoozFlag == 0))                                                                 //THIS IS FOR THE SNOOZ WHEN THE ALARM GOES OFF
    {
        delay_ms(DEBOUN);                                                                                                                        //delay for debounce
        while(!(SDOWN_PORT -> IN & SDOWN_PIN)){}                                                                                                 //if button is being held
        if(now.min < 55)                                                                                                                                                //BASICLY NORMAL ADD 5
        {
        snooz.min = now.min + 5;                                                                                                                    //ADDS 5 TO THE MINUTES
        snooz.hour = now.hour;
        }
        if(now.min >= 55)                                                                                                                           //THIS LOGIC TAKES CARE OF WHEN IT NEEDS TO ROLL OVER TO THE NEXT HOUR
        {
            snooz.hour = now.hour + 1;
            snooz.min = now.min - 55;
        }
        state = NORMAL_CLOCK;                                                                                                                       //SETS STATE BACK TO NORMAL
        snoozFlag = 1;                                                                                                                              //SETS SNOOZ FLAG
        bright = 0;                                                                                                                                     //MAKES LEDS TURN OFF
        TIMER_A0 -> CCR[WAKE_INST] = bright;                                                                                                        //TO TURN LEDS OFF
        MinSecFlag = 0;                                                                                                                             //TO MAKE SURE THIS IS OFF
    }
    if(!(BUTTON_PORT -> IN & BUTTON_PIN) && (degreeFlag == 0))                                                                                      //THIS IS FOR THE FEHRENHIGHT
        {
            delay_ms(DEBOUN);                                                                                                                        //delay for debounce
            while(!(BUTTON_PORT -> IN & BUTTON_PIN)){}
            degreeFlag = 1;
            MinSecFlag = 0;
        }
        if(!(BUTTON_PORT -> IN & BUTTON_PIN) && (degreeFlag == 1))                                                                                      //THIS IS FOR THE DEGREES IN CELCIUS
            {
                delay_ms(DEBOUN);                                                                                                                       //delay for debounce
                while(!(BUTTON_PORT -> IN & BUTTON_PIN)){}
                degreeFlag = 0;
                MinSecFlag = 0;
            }

                                                                                                                                                    //THESE CLEAR OUT THE INTERRUPTS FOR THE BUTTONS AFTER IT COMES THREW
   SET_PORT -> IFG &= ~SET_PIN;                                                                                                                     //turns flag to 0
   SDOWN_PORT -> IFG &= ~SDOWN_PIN;
   ONOFFUP_PORT -> IFG &= ~ONOFFUP_PIN;
   SETALARM_PORT -> IFG &= ~SETALARM_PIN;                                                                                                           //turns flag to 0
   MINSEC_PORT -> IFG &= ~MINSEC_PIN;                                                                                                                   //turns flag to 0
   BUTTON_PORT -> IFG &= ~BUTTON_PIN;                                                                                                                   //turns flag to 0
}

/*
 * THIS SETS THE RTC TIME WHEN AND AFTER THE TIME HAS BEEN DECIDED IN THE PORT INTERRUPT
 */
void setRTC(void)
{
    RTC_C -> TIM0 = set.min<<8 | set.sec;
    RTC_C -> TIM1 = 00<<8 | set.hour;
}

/*
 * THIS IS FOR THE TIME SET ABILITY THREW SERIAL COMUNICATION AND THREW THE DEVICE
 * THIS FUNCTION CONVERTS THE MILITARY TIME THATS USED BY THE SOFTWARE TO A STRING TO DISPLAY
 * THIS LOGIC IS USED THREW THE PROGRAM
 * ITS EXACLY THE SAME IN EXCEPTION OF WHAT VARIBLES THERE ARE
 * ITS DISTINGUISHES FROM AM AND PM ASWELL AND COPY TO THE STRING TO THEN BE DISPLAYED
 * THE SAME STRING IS USED THREW THESE TYPES OF FUNCTIONS THAT CONVERTS THE NUMBER OT THE STRING
 */
void convertTimeSet (void)
{

    if(set.hour == 0)
    {
        timeDisp[0] = '1';
        timeDisp[1] = '2';
        AmPmFlag = 0;
    }
    if((set.hour == 12))
    {
        timeDisp[0] = ((set.hour/10)%10)+48;
        timeDisp[1] = (set.hour%10)+48;
        AmPmFlag = 1;
    }
    if((set.hour < 10) && (set.hour != 0))
    {
       timeDisp[0] = ' ';
       timeDisp[1] = (set.hour+48);
       AmPmFlag = 0;
    }
    if((set.hour >= 10) && (set.hour < 12))
    {
        timeDisp[0] = ((set.hour/10)%10)+48;
        timeDisp[1] = (set.hour%10)+48;
        AmPmFlag = 0;
    }
    if((set.hour > 21)&&(set.hour < 24))
    {
        timeDisp[0] = (((set.hour-12)/10)%10)+48;
        timeDisp[1] = ((set.hour-12)%10)+48;
        AmPmFlag = 1;
    }
    if((set.hour > 12)&&(set.hour<22))
    {
        timeDisp[0] = ' ';
        timeDisp[1] = ((set.hour-12)%10)+48;
        AmPmFlag = 1;
    }
    if(set.min < 10)
    {
        timeDisp[3] = '0';
        timeDisp[4] = (set.min+48);
    }
    if(set.min >= 10)
    {
        timeDisp[3] = ((set.min/10)%10)+48;
        timeDisp[4] = (set.min%10)+48;
    }
    if(set.sec < 10)
    {
        timeDisp[6] = '0';
        timeDisp[7] = (set.sec+48);
    }
    if(set.sec >= 10)
    {
        timeDisp[6] = ((set.sec/10)%10)+48;
        timeDisp[7] = (set.sec%10)+48;
    }
    if(AmPmFlag == 0)                                                                                                   //COPYS AM IF THE FLAG IS NOT SET
    {
        timeDisp[8] = ' ';
        timeDisp[9] = 'A';
        timeDisp[10] = 'M';
    }
    if(AmPmFlag == 1)                                                                                                           //COPYS PM IF THE FLAG IS SET
    {
        timeDisp[8] = ' ';
        timeDisp[9] = 'P';
        timeDisp[10] = 'M';
    }
}

/*
 * THIS IS FOR THE ALARM SET ABILITY THREW SERIAL COMUNICATION AND THREW THE DEVICE
 * THIS FUNCTION CONVERTS THE MILITARY TIME THATS USED BY THE SOFTWARE TO A STRING TO DISPLAY
 * THIS LOGIC IS USED THREW THE PROGRAM
 * ITS EXACLY THE SAME IN EXCEPTION OF WHAT VARIBLES THERE ARE
 * ITS DISTINGUISHES FROM AM AND PM ASWELL AND COPY TO THE STRING TO THEN BE DISPLAYED
 * THE SAME STRING IS USED THREW THESE TYPES OF FUNCTIONS THAT CONVERTS THE NUMBER OT THE STRING
 */
void convertAlarmSet(void)
{

    if(alarm.hour == 0)
    {
        timeDisp[0] = '1';
        timeDisp[1] = '2';
        AmPmFlag = 0;
    }
    if((alarm.hour == 12))
    {
        timeDisp[0] = ((alarm.hour/10)%10)+48;
        timeDisp[1] = (alarm.hour%10)+48;
        AmPmFlag = 1;
    }
    if((alarm.hour < 10) && (alarm.hour != 0))
    {
       timeDisp[0] = ' ';
       timeDisp[1] = (alarm.hour+48);
       AmPmFlag = 0;
    }
    if((alarm.hour >= 10) && (alarm.hour < 12))
    {
        timeDisp[0] = ((alarm.hour/10)%10)+48;
        timeDisp[1] = (alarm.hour%10)+48;
        AmPmFlag = 0;
    }
    if((alarm.hour > 21)&&(alarm.hour < 24))
    {
        timeDisp[0] = (((alarm.hour-12)/10)%10)+48;
        timeDisp[1] = ((alarm.hour-12)%10)+48;
        AmPmFlag = 1;
    }
    if((alarm.hour > 12)&&(alarm.hour<22))
    {
        timeDisp[0] = ' ';
        timeDisp[1] = ((alarm.hour-12)%10)+48;
        AmPmFlag = 1;
    }
    if(alarm.min < 10)
    {
        timeDisp[3] = '0';
        timeDisp[4] = (alarm.min+48);
    }
    if(alarm.min >= 10)
    {
        timeDisp[3] = ((alarm.min/10)%10)+48;
        timeDisp[4] = (alarm.min%10)+48;
    }

    timeDisp[5] = ' ';
    timeDisp[6] = ' ';
    timeDisp[7] = ' ';

    if(AmPmFlag == 0)                                                                                               //COPYS AM IF THE FLAG IS NOT SET
    {
        timeDisp[8] = ' ';
        timeDisp[9] = 'A';
        timeDisp[10] = 'M';
    }
    if(AmPmFlag == 1)                                                                                               //COPYS PM IF THE FLAG IS SET
    {
        timeDisp[8] = ' ';
        timeDisp[9] = 'P';
        timeDisp[10] = 'M';
    }

}

/*
 * THIS CHECKS TO SEE IF THE STRING ENTERD INTO THE SERIAL MONITOR WAS A VALID INPUT
 * ITLL SET THE FLAG THAT IS NECISSARY TO CONTINUE THE PROGRAM WITH THE AJUSTMENTS THAT ARE DESIRED
 */
void checkStr(char *string)
{
    if(string[0] != '\0')                                                                                               //if the first char is a null it wont do anything
           {
               if((strlen(string)==16)&&(string[0] == 'S')&&(string[1] == 'E')&&(string[2] == 'T')&&(string[3] == 'T')&&(string[4] == 'I')&&(string[5] == 'M')&&(string[6] == 'E')&&(string[7] == ' ')&&(string[10] == ':')&&(string[13] == ':'))//if it is 16 chars long for SETTIME HH:MM:SS
               {                                                                                                        //LOGIC TO MAKE SURE THE INPUT IS RIGHT
                   if( ((string[8] >= '0')&&(string[8] <= '2')) && ((string[9] >= '0')&&(string[9] <= '9')) && ((string[11] >= '0')&&(string[11] <= '5')) && ((string[12] >= '0')&&(string[12] <= '9')) && ((string[14] >= '0')&&(string[14] <= '5')) && ((string[15] >= '0')&&(string[15] <= '9')))
                   {                                                                                                    //LOGIC TO MAKE SURE THE INPUT IS RIGHT
                       if((string[8] == '2') && ((string[9] >= '0')&&(string[9] <= '3')))
                       {                                                                                                //LOGIC TO MAKE SURE THE INPUT IS RIGHT
                   sprintf(terminal, "Valid %s\n", string);                                                             //USING STRING FUNCTIONS
                   writeOutput(terminal);                                                                               //WRITES TO SERIAL
                   validFlag = 1;                                                                                       //VALID FLAG
                   sTvalidFlag = 1;                                                                                     //SET TIME FLAG
                       }
                       else if (!(string[8] == '2'))
                       {                                                                                                //LOGIC TO MAKE SURE THE INPUT IS RIGHT
                           sprintf(terminal, "Valid %s\n", string);                                                     //USING STRING FUNCTIONS
                           writeOutput(terminal);                                                                       //WRITES TO SERIAL
                           validFlag = 1;                                                                               //VALID FLAG
                           sTvalidFlag = 1;                                                                             //SET TIME FLAG
                       }
                   }
               }
               if((strlen(string)==14)&&(string[0] == 'S')&&(string[1] == 'E')&&(string[2] == 'T')&&(string[3] == 'A')&&(string[4] == 'L')&&(string[5] == 'A')&&(string[6] == 'R')&&(string[7] == 'M')&&(string[8] == ' ')&&(string[11] == ':'))//if it 14 Chars Long for SETALARM HH:MM
               {                                                                                                        //LOGIC TO MAKE SURE THE INPUT IS RIGHT
                   if( ((string[9] >= '0')&&(string[9] <= '2')) && ((string[10] >= '0')&&(string[10] <= '9')) && ((string[12] >= '0')&&(string[12] <= '5')) && ((string[13] >= '0')&&(string[13] <= '9')))
                   {                                                                                                    //LOGIC TO MAKE SURE THE INPUT IS RIGHT
                       if((string[9] == '2') && ((string[10] >= '0')&&(string[10] <= '3')))
                       {                                                                                                //LOGIC TO MAKE SURE THE INPUT IS RIGHT
                   sprintf(terminal, "Valid %s\n", string);                                                             //USING STRING FUNCTIONS
                   writeOutput(terminal);
                   validFlag = 1;                                                                                       //VALID FLAG
                   sAvalidFlag = 1;                                                                                     //THIS IS TO SET ALARM FLAG
                       }
                       else if (!(string[9] == '2'))
                       {                                                                                                //IF ITS 2 IT CANT BE 3 OR MORE
                           sprintf(terminal, "Valid %s\n", string);                                                     //USING STRING FUNCTIONS
                           writeOutput(terminal);
                           validFlag = 1;                                                                               //VALID FLAG
                           sAvalidFlag = 1;                                                                             //SET THE SET ALARM FLAG
                       }
                   }
               }
               if((strlen(string)==8)&&!(strcmp(string,readT)))                                                         //if it 8 Chars Long for READTIMEAND THEN DISPLAYES IT TO THE SERIAL MONITOR
               {
                   sprintf(terminal, "\n%s-->\n", string);                                                              //USING STRING FUNCTIONS
                   writeOutput(terminal);
                   validFlag = 1;                                                                                       //VALID FLAG
                   rTvalidFlag = 1;                                                                                     //SETS READ TIME FLAG
               }
               if((strlen(string)==9)&&!(strcmp(string,readA)))                                                         //if it 9 Chars Long for READALARM AND THEN DISPLAYES IT TO THE SERIAL MONITOR
               {
                   sprintf(terminal, "\n%s-->", string);                                                                //USING STRING FUNCTIONS
                   writeOutput(terminal);                                                                               //WRITES TO TERMINAL
                   validFlag = 1;                                                                                       //VALID FLAG
                   rAvalidFlag = 1;                                                                                     //SETS READ ALARM FLAG
               }
               else if(validFlag == 0)                                                                                  //anything other than the rest BECAUSE THIS MAKES IT WRONG IF IT ISNT ANY OF THOESE
                              {
                               sprintf(terminal, "Invalid %s\n", string);                                               //USING STRING FUNCTIONS
                               writeOutput(terminal);                                                                   //WRITES TO TERMINAL
                               validFlag = 0;                                                                           //MEANS THE INPUT IS WRONG
                               }
           }                                                                                                            //null if
}                                                                                                                       //function

/*
 * THIS TAKES THE ALARM TIME ENTERED INTO THE SERIAL MONITOR AND SETS THE NEW ALARM HOUR AND MIN
 */
void convertSerialAlarm(char *string)
{
    alarm.hour = (string[10]-48) + ((string[9]-48)*10);
    alarm.min = (string[13]-48) + ((string[12]-48)*10);
}

/*
 * TAKES THE CHARACTERS ENTERED FROM THE SERIAL MONITOR AND CONVERTS IT TO
 * SET THE TIME IN THE RTC THAT GOING TO BE DISPLAYED
 * SAME AS ABOVE BUT FOR THE TIME
 */
void convertSerialTime(char *string)
{
    set.hour = (string[9]-48) + ((string[8]-48)*10);
    set.min = (string[12]-48) + ((string[11]-48)*10);
    set.sec = (string[15]-48) + ((string[14]-48)*10);

    RTC_C -> TIM0 = set.min<<8 | set.sec;
    RTC_C -> TIM1 = 00<<8 | set.hour;
}

/*
 * THIS CONVERTS THE TIME THAT IT IS TO BE DISPLAYED ON THE SERIAL MONITOR
 * THIS LOGIC IS USED WIDLY THREW THE PROGRAM
 */
void serialTime(void)
{
    if(now.hour < 10)
    {
        timeDisp[0] = '0';
        timeDisp[1] = (now.hour%10)+48;
    }
    if(now.hour >= 10)
    {
            timeDisp[0] = ((now.hour/10)%10)+48;
            timeDisp[1] = (now.hour%10)+48;
    }
        if(now.min < 10)
        {
            timeDisp[3] = '0';
            timeDisp[4] = (now.min+48);
        }
        if(now.min >= 10)
        {
            timeDisp[3] = ((now.min/10)%10)+48;
            timeDisp[4] = (now.min%10)+48;
        }
        if(now.sec < 10)
        {
            timeDisp[6] = '0';
            timeDisp[7] = (now.sec+48);
        }
        if(now.sec >= 10)
        {
            timeDisp[6] = ((now.sec/10)%10)+48;
            timeDisp[7] = (now.sec%10)+48;
        }
        timeDisp[5] = ':';
        timeDisp[8] = ' ';
        timeDisp[9] = ' ';
        timeDisp[10] = ' ';

}

/*
 * THIS IS A CONVERSION FROM THE ACUAL ALARM TIME TO BE DISPLAYED IN THE SERIAL MONITOR
 * THIS LOGIC IS USED THREW THE PROGRAM
 */
void serialAlarm(void)
{
    if(alarm.hour < 10)
    {
        timeDisp[0] = '0';
        timeDisp[1] = (alarm.hour%10)+48;
    }
    if(alarm.hour >= 10)
    {
            timeDisp[0] = ((alarm.hour/10)%10)+48;
            timeDisp[1] = (alarm.hour%10)+48;
    }
        if(alarm.min < 10)
        {
            timeDisp[3] = '0';
            timeDisp[4] = (alarm.min+48);
        }
        if(alarm.min >= 10)
        {
            timeDisp[3] = ((alarm.min/10)%10)+48;
            timeDisp[4] = (alarm.min%10)+48;
        }

        timeDisp[5] = ' ';
        timeDisp[6] = ' ';
        timeDisp[7] = ' ';
        timeDisp[8] = ' ';
        timeDisp[9] = ' ';
        timeDisp[10] = ' ';

}

/*
 * THIS IS SET UP TO WRITE USEING UARTS TO COMUNICATE WITH THE COMPUTER
 * THIS IS THREW THE TX PIN ON P1.3
 */
void writeOutput(char *string)
{
    int i = 0;                                                                          // Location in the char array "string" that is being written to

    while(string[i] != '\0') {
        EUSCI_A0->TXBUF = string[i];
        i++;
        while(!(EUSCI_A0->IFG & BIT1));
    }
}

/*
 * THIS READS THE INPUT FROM THE INPUTBUFFER STRING AND SET IT EQUAL TO THE STRING
 * THIS ONLY HAPPENS WHEN THE \N HAS BEEN PUT INTO THE TERMINAL
 * THIS USES A DO WHILE LOOP WHICH IS PROBALY NOT NECISSARY IN THIS INSTANCE , AS USAUALL, BUT IT WORKS
 *
 */
void readInput(char *string)
{
    int i = 0;                                                                                // Location in the char array "string" that is being written to
       do
       {
                                                                                               // If a new line hasn't been found yet, but we are caught up to what has been received, wait here for new data
           while(read_location == storage_location && INPUT_BUFFER[read_location] != '\n');
           string[i] = INPUT_BUFFER[read_location];                                             // Manual copy of valid character into "string"
           INPUT_BUFFER[read_location] = '\0';
           i++;                                                                                  // Increment the location in "string" for next piece of data
           read_location++;                                                                      // Increment location in INPUT_BUFFER that has been read
           if(read_location == BUFFER_SIZE)                                                              // If the end of INPUT_BUFFER has been reached, loop back to 0
               read_location = 0;
       }
       while(string[i-1] != '\n');                                                                               // If a \n was just read, break out of the while loop

       string[i-1] = '\0';                                                                            // Replace the \n with a \0 to end the string when returning this function
}

/*
 * THIS HANDELS THE ESUCI INTTURUPT
 * BASICLY THIS ALOUWS WHATEVER ENTERED INTO THE SERIAL MONITOR TO BE SAVED INTO THE INPUT BUFFER STRING
 *
 * WHEN THE NEWLINE CHARACTER IS PRESSED ITLL SET A FLAG TO INSINUATE THAT SOMEONE TRIED TO INPUT SOMETHING
 * GIVEN DURING LECTURE
 */
void EUSCIA0_IRQHandler(void)
{
    if (EUSCI_A0->IFG & BIT0)                                                                   // Interrupt on the receive line
    {
        INPUT_BUFFER[storage_location] = EUSCI_A0->RXBUF;                                               // store the new piece of data at the present location in the buffer
        EUSCI_A0->IFG &= ~BIT0;                                                                     // Clear the interrupt flag right away in case new data is ready
        if(INPUT_BUFFER[storage_location] == '\n')
            SerialCom = 1;
        storage_location++;                                                                     // update to the next position in the buffer
        if(storage_location == BUFFER_SIZE)                                                     // if the end of the buffer was reached, loop back to the start
        {
            storage_location = 0;
        }
    }
}

/*
 * THIS IS TO SETS UP THE SERIAL COMUNICATION (UARTS)
 * THIS IS
 * 9600
 * ONE STOP BIT
 * NO PARITY
 *
 *
 * ALSO ENABLES INTERRUPT ON eusci
 *
 * GIVEN DURING LECTURE
 */
void setupSerial()
{
                                                                                                    //one stop bit no parity 9600
    P1->SEL0 |=  (BIT2 | BIT3);                                                                     // P1.2 and P1.3 are EUSCI_A0 RX
    P1->SEL1 &= ~(BIT2 | BIT3);                                                                     // and TX respectively.

    EUSCI_A0->CTLW0  = BIT0;                                                                        // Disables EUSCI. Default configuration is 8N1
    EUSCI_A0->CTLW0 |= BIT7;                                                                        // Connects to SMCLK BIT[7:6] = 10
    EUSCI_A0->CTLW0 |= 0x0001;                                                                      //BIT15 = no Parity, BIT11 = One Stop Bit
                                                                                                    // Baud Rate Configuration
                                                                                                    // 3000000/(16*9600) = 19.53125
                                                                                                    // UCBR  = 19
                                                                                                    // UCBRF = .53125 * 16 = 8 (0.5) (Remainder of the divide)
                                                                                                    // UCBRS = 3000000/9600 remainder=0.5 -> 0xAA (look up table 22-4)
    EUSCI_A0->BRW = 19;
    EUSCI_A0->MCTLW = 0xAA81;                                                                       //found above

    EUSCI_A0->CTLW0 &= ~BIT0;                                                                       // Enable EUSCI
    EUSCI_A0->IFG &= ~BIT0;                                                                         // Clear interrupt
    EUSCI_A0->IE |= BIT0;                                                                           // Enable interrupt
    NVIC_EnableIRQ(EUSCIA0_IRQn);
}
