#ifndef __LCD_H__
#define __LCD_H__

#define PULSE_PORT P4           //port the enable pin is tied to
#define PULSE_PIN BIT2          //pin the enable pin is tied to

#define DATA_PORT P4            //data port for pins DB7-DB4
#define DATA_PIN 0xF0;          //DB7-DB4 are on the respected pins

#define RS_PORT P4              //read pins port
#define RS_PIN BIT3             //read pins pin

void initLCDPins(void);         //Function prototypes for the LCD
void initLCD(void);
void PulsEnablePin(void);
void pushNib(uint8_t nib);
void pushByt(uint8_t byte);
void comWrit(uint8_t com);
void dataWrit(uint8_t data);

#endif
