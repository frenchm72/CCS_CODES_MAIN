/****************************************************************************
  Title:       EGR 226
  Filename:
  Author(s):   Mathew Yerian-French
  Date:        11/11/2018
  Instructor:  Professor Scott Zuidema
  Description:
  Note:
*****************************************************************************/
#include "msp.h"
#include "temp.h"
#include "promain.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*
 * INITS THE adc TO READ FORM MULTIPLE ADC PINS
 */
    void ADC14init(void)
    {
        ADC_PORT -> SEL0 |= ADC_PIN;//set adc conversion
        ADC_PORT -> SEL1 |= ADC_PIN;

        LCDADC_PORT -> SEL0 |= LCDADC_PIN;//set adc conversion
        LCDADC_PORT -> SEL1 |= LCDADC_PIN;

        ADC14 -> CTL0 &= ~ADC14_CTL0_ENC;//disable conersion

       /* ADC14 -> CTL0 |= ADC14_CTL0_PDIV__32 |//setpdiv to 32
                         ADC14_CTL0_SHP |//hold off sampling timer
                         ADC14_CTL0_SSEL__SMCLK |//sub master clock
                         ADC14_CTL0_SHT1__32 |//hol regisert for 32 clock
                         ADC14_CTL0_ON;//turns on*/
        ADC14->CTL0         =    0b10000100001000100000001110010000;

        //ADC14 -> CTL1 |= ADC14_CTL1_RES__14BIT;//10 but resolution
        ADC14->CTL1         =    BIT4|BIT5;         // Bits 5 and 4 = 11 to enable 14 bit conversion

        ADC14 -> MCTL[0] = ADC_INST;//defult config for adc
        ADC14->MCTL[1] = LCDADC_INST | BIT7;

        ADC14 -> IER0 = 0;//interrupt on

        ADC14 -> CTL0 |= ADC14_CTL0_ENC;//enable conversion
    }

    /*
     * GETS THE TEMP AND SENDS BACK A FLOAT
     */
    float getTemp(int degree)
    {
        uint32_t adc_input;
        float TEMP, V_in;

        ADC14 -> CTL0 |= ADC14_CTL0_SC; //starts conversion

        adc_input = ADC14->MEM[0];
        V_in = ((V_REF * ((adc_input)))/pow(2,14));
        TEMP = ((V_in*1000))/10;//converts to celsius
        if(degree == 1)
                        {
                        TEMP = (TEMP *(9.0/5.0))+32.0;
                        }
        return TEMP;
    }

    /*
     * GETS THE DISPLAY BRIGHTNESS FROM THE POT AND THEN ADJUST THE pwm FOR TIMERA0
     */
void LCDbrightUpdate(void)
{
    uint32_t c_input;
    float max, V;
    max = MAXBRIGHT;

    ADC14 -> CTL0 |= ADC14_CTL0_SC; //starts conversion

    c_input = ADC14->MEM[1];
    V = ((V_REF * ((c_input)))/pow(2,14));

    TIMER_A0 -> CCR[LCDBRIGHT_INST] = max * (V/V_REF);
}
