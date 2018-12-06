#ifndef __MOTOR_H__
#define __MOTOR_H__

#define TA0_PERIOD 7499 //30k tic corrasponds to 2.5ms period 400Hz

#define MOTOR_PORT P2
#define MOTOR_PIN BIT4
#define MOTOR_INST 1

//#define MOTOSPD 7499;

void initTimerA(void);
void relTimer(void);
void delay_ms(int ms);
void initMotoPWM(void);
void relMotoPWM(void);
void setMotoPWM(int spee);

#endif
