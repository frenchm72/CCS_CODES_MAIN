#ifndef __PROMAIN_H__
#define __PROMAIN_H__

#define TEMPSTR 6//TEMP LENGTH
#define TIMESTR 11//TIME TO DISPLAY LENGTH

#define ONOFFUP_PORT P1//ON OFF UP PIN
#define ONOFFUP_PIN BIT6

#define SDOWN_PORT P1// SNOOZ AND DOWN PIN
#define SDOWN_PIN BIT7

#define SETALARM_PORT P1//SET ALARM PIN
#define SETALARM_PIN BIT0

#define SET_PORT P1//SET TIME PIN
#define SET_PIN BIT5

#define MINSEC_PORT P1//INCREASE BY MINUTE PER SECOND
#define MINSEC_PIN BIT4

#define WAKE_PORT P2//WAKE UP LIGHTS PIN
#define WAKE_PIN BIT4
#define WAKE_INST 1

#define LCDBRIGHT_PORT P2//LCD PWM PIN
#define LCDBRIGHT_PIN BIT5
#define LCDBRIGHT_INST 2

#define MAXBRIGHT 7499//MAX BRIGHT

#define DEBOUN 100//DEBOUNCE

void displayTime(void);//PROTOTYPES
void displayTemp(void);
void convertTime(void);
void convertTimeSet(void);
void convertAlarmSet(void);

void initPins(void);

void delay_micro(unsigned mic);
void delay_ms(unsigned ms);
void SysTick_Init(void);

void configRTC(void);
void setRTC(void);

#endif
