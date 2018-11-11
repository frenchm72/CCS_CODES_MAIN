#ifndef __16x4LCD_H__
#define __16x4LCD_H__

#define PULSE_PORT P4
#define PULSE_PIN BIT2

#define DATA_PORT P4
#define DATA_PIN 0xF0;
#define RS_PORT P4
#define RS_PIN BIT1

void initPins(void);
void initLCD(void);
void delay_micro(unsigned mic);
void delay_ms(unsigned ms);
void PulsEnablePin(void);
void pushNib(uint8_t nib);
void pushByt(uint8_t byte);
void comWrit(uint8_t com);
void dataWrit(uint8_t data);
void StsTick_Init(void);
void shift_off(char lab[], char spac[]);
void shift_on(char lab[]);

#endif
