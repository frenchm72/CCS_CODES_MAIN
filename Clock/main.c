#include "msp.h"
#include <stdio.h>
#include <stdlib.h>

struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
}now;

int RTCFlag = 0;

void configRTC(void);
void printRTC(void);

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	__disable_irq();
	configRTC();
	NVIC_EnableIRQ(RTC_C_IRQn);
    __enable_irq();

    while(1)
    {
if(RTCFlag)
{
    printRTC();
    RTCFlag = 0;
}
    }
}

void configRTC(void)
{

    RTC_C -> CTL0 = 0xA510;
    RTC_C -> CTL13 = 0x0000;
    RTC_C -> TIM0 = 59<<8 | 55;
    RTC_C -> TIM1 = 00<<8 | 23;

}
void printRTC(void)
{
printf("%02d:%02d:%02d\n", now.hour,now.min,now.sec);
}

void RTC_C_IRQHandler(void)
{
    now.sec = RTC_C -> TIM0>>0 & 0x00FF;
    now.min = RTC_C -> TIM0>>8 & 0x00FF;
    now.hour = RTC_C -> TIM1>>0 & 0x00FF;

    RTCFlag = 1;
    RTC_C -> CTL0 = 0xA510;
}
