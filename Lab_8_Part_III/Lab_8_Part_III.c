/****************************************************************************
  Title:       EGR 226 Lab 6
  Filename:    Lab_6_Part_II
  Author(s):   Mathew Yerian-French & Malcolm
  Date:        09/30/2018
  Instructor:  Professor Scott Zuidema
  Description:
  Note:
*****************************************************************************/

#include "msp.h"
#include "LCD.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

    uint32_t adc_input;
    uint8_t i;
    float V_REF = 3.3, V_in, TEMP;
    int degree = 0;

    char hder[18] = {'V','o','l','t',' ','I','n',' ',' ',' ',' ',' ','T','e','m','p','C','F',},
         V_IN[5],TemP[5];

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    __disable_irq();
    SysTick_Init();
    ADC14init();
    initPins();//pin initiliz
    delay_ms(60);
    initLCD();
    delay_ms(160);
    NVIC -> ISER[0] |= 1 << ADC14_IRQn;
    NVIC -> ISER[1] |= 1 <<((PORT1_IRQn)&31);
    __enable_irq();
    delay_ms(160);

    while(1)
    {
        TEMP = 0;
        comWrit(0x01);
        delay_micro(100);
        comWrit(0x01);
        delay_micro(100);
        ADC14 -> CTL0 |= ADC14_CTL0_SC; //starts conversion
        adc_input = ADC14->MEM[0];
        V_in = ((V_REF * ((adc_input)))/pow(2,14));
        TEMP = ((V_in*1000))/10;//converts to celsius

        if(degree == 1)
                {
                TEMP = (TEMP *(9.0/5.0))+32.0;
                }
        convertVal();
    comWrit(0x80);//address  05
     delay_micro(100);
    for(i=0;i<16;i++)
    {
        dataWrit(hder[i]);
    }

    comWrit(0xC0);
    delay_micro(100);
    for(i=0;i<5;i++)
       {
           dataWrit(V_IN[i]);
       }
    comWrit(0xC9);
    delay_micro(100);
    for(i=0;i<5;i++)
       {
           dataWrit(TemP[i]);
       }
    dataWrit(0b11011111);//for the degree symbol
    if(degree == 0)
    {
    dataWrit(hder[16]);//celsius
    }
    if(degree == 1)
        {
        dataWrit(hder[17]);//fahrenheit
        }
    delay_ms(2000);
    }
}
void convertVal(void)//converts the number to a string
{
    int v, t;
    v = V_in * 1000;
    t = TEMP * 100;

    V_IN[0] = (v/1000)+48;
    V_IN[1] = '.';
    V_IN[2] = ((v/100)%10)+48;
    V_IN[3] = ((v/10)%10)+48;
    V_IN[4] = (v%10)+48;

    TemP[0] = (t/1000)+48;
    TemP[1] = ((t/100)%10)+48;
    TemP[2] = '.';
    TemP[3] = ((t/10)%10)+48;
    TemP[4] = (t%10)+48;
}
void ADC14init(void)
{
    ADC_PORT -> SEL0 |= ADC_PIN;//set adc conversion
    ADC_PORT -> SEL1 |= ADC_PIN;
    ADC14 -> CTL0 &= ~ADC14_CTL0_ENC;//enable conersion
    ADC14 -> CTL0 |= ADC14_CTL0_PDIV__32 |//setpdiv to 32
                     ADC14_CTL0_SHP |//hold off sampling timer
                     ADC14_CTL0_SSEL__SMCLK |//sub master clock
                     ADC14_CTL0_SHT1__32 |//hol regisert for 32 clock
                     ADC14_CTL0_ON;//turns on
    ADC14 -> CTL1 |= ADC14_CTL1_RES__14BIT;//10 but resolution
    ADC14 -> MCTL[ADC_INST] = 0;//defult config for adc
    ADC14 -> IER0;//interrupt on
    ADC14 -> CTL0 |= ADC14_CTL0_ENC;//enable conversion
}
void BUTTON_IN(void)                                                                                                                //button port interrupt
{
    BUTTON_PORT -> IFG & BUTTON_PIN;
    if(!degree && !(BUTTON_PORT -> IN & BUTTON_PIN))
    {
        delay_ms(25);                                                                                                              //delay for debounce
        while(!(BUTTON_PORT -> IN & BUTTON_PIN)){}
        degree = 1;                                                                                                              //to figure what mode its in
    }
    if(degree && !(BUTTON_PORT -> IN & BUTTON_PIN))
        {
        delay_ms(25);                                                                                                              //delay for debounce
        while(!(BUTTON_PORT -> IN & BUTTON_PIN)){}                                                                                  //if button is being held
            degree = 0;
        }
   BUTTON_PORT -> IFG &= ~BUTTON_PIN;                                                                                              //turns flag to 0
}
