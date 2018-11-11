#ifndef __LCD_H__
#define __LCD_H__

#define PULSE_PORT P4
#define PULSE_PIN BIT2

#define DATA_PORT P4
#define DATA_PIN 0xF0;
#define RS_PORT P4
#define RS_PIN BIT1

#define ADC_PORT P5
#define ADC_PIN BIT5
#define ADC_INST 0

#define BUTTON_PORT P1
#define BUTTON_PIN BIT5
#define BUTTON_IN   PORT1_IRQHandler    //port handler

void initPins(void);
void initLCD(void);
void delay_micro(unsigned mic);
void delay_ms(unsigned ms);
void PulsEnablePin(void);
void pushNib(uint8_t nib);
void pushByt(uint8_t byte);
void comWrit(uint8_t com);
void dataWrit(uint8_t data);
void SysTick_Init(void);
void convertVal(void);
void ADC14init(void);

#endif
