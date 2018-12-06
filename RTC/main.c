#include "msp.h"
#include <stdio.h>
#include <string.h>

void configRTC(void);
void printRTC(void);
void month(char* monthStr, uint8_t mon_num);
void dow(char* dowStr, uint8_t day_num);

// global struct variable called now
struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t mon;
    uint16_t year;
    uint8_t dow;
} now;

uint8_t RTC_flag = 0;

void main(void)
{
     WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer
__disable_irq();
configRTC();
NVIC_EnableIRQ(RTC_C_IRQn);
__enable_irq();
 while(1)
 {
     if(RTC_flag)
     {
         printRTC();
         RTC_flag = 0;
     }
}
}
void configRTC(void)
        {
    RTC_C‐>CTL0  =  0xA510; //Write Code, IE on RTC Ready
    RTC_C‐>CTL13  =  0x0000;
  RTC_C‐>TIM0  = 30<<8 | 00;
  RTC_C‐>TIM1  = 2<<8 | 12;
  RTC_C‐>DATE  = 11<<8 | 26;
  RTC_C‐>YEAR  = 2018;
        }
void printRTC(void)
        {
    char monthStr[5];
    char dowStr[5];
 month(monthStr, now.mon);
 dow(dowStr, now.dow);
 printf("%02d:%02d:%02d %s %d %s %4d\n",now.hour, now.min, now.sec, dowStr, now.day, monthStr, now.year);
}
void RTC_C_IRQHandler(void)
        {
    now.sec  =  RTC_C‐>TIM0>>0 & 0x00FF;
    now.min  =  RTC_C‐>TIM0>>8 & 0x00FF;
    now.hour  =  RTC_C‐>TIM1>>0 & 0x00FF;
    now.dow =  RTC_C‐>TIM1>>8 & 0x00FF;
    now.day =  RTC_C‐>DATE>>0 & 0x00FF;
    now.mon = RTC_C‐>DATE>>8 & 0x00FF;
    now.year = RTC_C‐>YEAR;
    RTC_flag = 1;
    RTC_C‐>CTL0  =  0xA510;
        }
void month(char* monthStr,uint8_t mon_num)
        {
    switch (mon_num)
            {
    case 1:  strcpy(monthStr, "JAN");
    break;
    case 2:  strcpy(monthStr, "FEB");
    break;
    case 3:  strcpy(monthStr, "MAR");
    break;
    case 4:  strcpy(monthStr, "APR");
    break;
    case 5:  strcpy(monthStr, "MAY");
    break;
    case 6:  strcpy(monthStr, "JUN");
    break;
    case 7:  strcpy(monthStr, "JUL");
    break;
    case 8:  strcpy(monthStr, "AUG");
    break;
    case 9:  strcpy(monthStr, "SEP");
    break;
    case 10:  strcpy(monthStr, "OCT");
    break;
    case 11:  strcpy(monthStr, "NOV");
    break;
    case 12:  strcpy(monthStr, "DEC");
    break;
    default:  strcpy(monthStr, "‐‐‐");
    break;
            }
        }
void dow(char* dowStr,uint8_t dow_num)
        {
    switch (dow_num)
            {
    case 1:  strcpy(dowStr, "SUN");
    break;
    case 2:  strcpy(dowStr, "MON");
    break;
    case 3:  strcpy(dowStr, "TUE");
    break;
    case 4:  strcpy(dowStr, "WED");
    break;
    case 5:  strcpy(dowStr, "THU");
    break;
    case 6:  strcpy(dowStr, "FRI");
    break;
    case 7:  strcpy(dowStr, "SAT");
    break;
    default:  strcpy(dowStr, "‐‐‐");
    break;
            }
        }
