#include "msp.h"
#include "LCD.h"
#include "promain.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void initLCD(void)
{
  comWrit(0x03);                                //REST 3 times
  delay_ms(10);
  comWrit(0x03);
  delay_ms(10);
  comWrit(0x03);
  delay_ms(10);

  comWrit(0x02);                                //4bit mode
  delay_micro(100);
  comWrit(0x02);
  comWrit(0x06);                                //2 line 5x7 format
  delay_micro(100);
  comWrit(0x0F);                                //display on cursor blinking
  delay_micro(100);
  comWrit(0x01);                                //clears display
  delay_micro(100);
  comWrit(0x06);                                //Increment cursor
  delay_micro(100);
}
void PulsEnablePin(void)
{
    PULSE_PORT -> OUT &= ~PULSE_PIN;            //this function will turn on and off the pin sending a pulse to the LCD
    delay_micro(10);
    PULSE_PORT -> OUT |= PULSE_PIN;
    delay_micro(10);
    PULSE_PORT -> OUT &= ~PULSE_PIN;
    delay_micro(10);
}
void pushNib(uint8_t nib)
{
    DATA_PORT -> OUT &= ~0xF0;                  //pushes the nib to be read by the LCD
    DATA_PORT -> OUT |= (nib & 0x0F)<<4;
    PulsEnablePin();                            //pulses the LCD to read
}
void pushByt(uint8_t byte)                      //push byte function
{
    uint8_t nib;

    nib = ( byte & 0xF0)>>4;                    //right shifts the bits
    pushNib(nib);                               //calls function to send to lcd

    nib = (byte & 0x0F);                        //send the first 4
    pushNib(nib);                               //sends to LCD
    delay_micro(100);
}
void comWrit(uint8_t com)
{
    RS_PORT -> OUT &= ~RS_PIN;                  //makes read pin 0
    pushByt(com);                               //sends the command
}
void dataWrit(uint8_t data)
{
    RS_PORT -> OUT |= RS_PIN;                   //makes read pin 1
    pushByt(data);                              //sends the data to the LCD
}
void initLCDPins(void)
{
    PULSE_PORT->SEL0 &= ~PULSE_PIN;             //setting up pulse pin
    PULSE_PORT->SEL1 &= ~PULSE_PIN;
    PULSE_PORT->DIR |= PULSE_PIN;
    PULSE_PORT->OUT |= PULSE_PIN;

    RS_PORT->SEL0 &= ~RS_PIN;                   //setting up RS pin
    RS_PORT->SEL1 &= ~RS_PIN;
    RS_PORT->DIR |= RS_PIN;
    RS_PORT->OUT |= RS_PIN;

    DATA_PORT->SEL0 &= ~DATA_PIN;               //setting up DATA pins
    DATA_PORT->SEL1 &= ~DATA_PIN;
    DATA_PORT->DIR |= DATA_PIN;
    DATA_PORT->OUT |= DATA_PIN;
}
