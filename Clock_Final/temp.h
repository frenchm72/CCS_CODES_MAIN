#ifndef __TEMP_H__
#define __TEMP_H__

#define ADC_PORT P5//FOR PIN FOR TEMP
#define ADC_PIN BIT5
#define ADC_INST 0

#define LCDADC_PORT P4//FOR THE POT FOR THE BIGHTNESS OF LCD
#define LCDADC_PIN BIT5
#define LCDADC_INST 8

#define BUTTON_PORT P1//TO CHANGE THE DEGREES BUTTON
#define BUTTON_PIN BIT1
#define BUTTON_IN   PORT1_IRQHandler    //port handler

#define V_REF 3.3

float getTemp(int degree);
void convertTemp(float TEMP);
void ADC14init(void);
void LCDbrightUpdate(void);

#endif
