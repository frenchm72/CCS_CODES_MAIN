#include "msp.h"
#include <stdio.h>
#include <math.h>

#define ADC_PORT P5
#define ADC_PIN BIT5
#define ADC_INST 0

void ADC14init(void);
void SysTickInit(void);
void delay_ms(int ms);

uint32_t adc_input;
float VOLT, V_REF = 3.3;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    __disable_irq();
    SysTickInit();
    ADC14init();

    NVIC -> ISER[0] |= 1 << ADC14_IRQn;
    __enable_irq();
    while(1)
    {
        ADC14 -> CTL0 |= ADC14_CTL0_SC; //starts conversion
        adc_input = ADC14->MEM[0];
        VOLT = V_REF * ((adc_input + 0.5)/pow(2,14));
        printf("VOLTAGE:  %f\t", VOLT);printf("VOLTAGE REF:  %2.1f\t\t", V_REF);printf("ADC INPUT:  %d\n", adc_input);
        delay_ms(250);
    }
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
void SysTickInit(void)
{
        SysTick -> CTRL = 0;//disable systick during step
        SysTick -> LOAD = 0x00FFFFFF;//max reload value
        SysTick -> VAL = 0;//clears it
        SysTick -> CTRL = 0x00000005;//enables systick 3MHz no interrupts
}
void delay_ms(int ms)
{
        SysTick->LOAD =(3000*ms)-1;
        SysTick->VAL = 0;
        while((SysTick->CTRL & BIT(16))==0);
}
