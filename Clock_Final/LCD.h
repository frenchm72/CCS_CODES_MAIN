#ifndef __LCD_H__
#define __LCD_H__

#define PULSE_PORT P6//PULSE PIN
#define PULSE_PIN BIT1

#define DATA_PORT P6//DATA PIN
#define DATA_PIN 0xF0;

#define RS_PORT P6//RS PIN
#define RS_PIN BIT0

void initLCD(void);//FUNCTION FOR LCD
void PulsEnablePin(void);
void pushNib(uint8_t nib);
void pushByt(uint8_t byte);
void comWrit(uint8_t com);
void dataWrit(uint8_t data);

#endif
