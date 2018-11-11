#include "msp.h"
#include "motor.h"
#include "promain.h"
#include <stdio.h>
#include <stdlib.h>

void initMOTOpins(void)
{
    MOTOR_PORT -> SEL0 &= ~MOTOR_PIN;                               //seting up dc motor pin
    MOTOR_PORT -> SEL1 &= ~MOTOR_PIN;
    MOTOR_PORT -> DIR  |=  MOTOR_PIN;

    SERVO_PORT->SEL0 &= ~SERVO_PIN;                                 //setting up servo pin
    SERVO_PORT->SEL1 &= ~SERVO_PIN;
    SERVO_PORT->DIR |=   SERVO_PIN;
}
void initTimerAmoto(void)
{
    TIMER_A0 -> CCR[0] = TA0_PERIOD;                                //sets up timer A0 to make a PWM for the dc motor
    TIMER_A0 -> CCR[MOTOR_INST] = 0;
    TIMER_A0 -> CCTL[MOTOR_INST] = TIMER_A_CCTLN_OUTMOD_7;          //mode 7
    TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 |
                      TIMER_A_CTL_MC_0 |
                      TIMER_A_CTL_MC__UP |                          //up mode
                      TIMER_A_CTL_CLR;                              //clears timer

    TIMER_A2 -> CCR[0] = SERVO_PERIOD;                              //sets up timer A0 to make a PWM for the servo position
        TIMER_A2 -> CCR[SERVO_INST] = 0;
        TIMER_A2 -> CCTL[SERVO_INST] = TIMER_A_CCTLN_OUTMOD_7;      //mode 7
        TIMER_A2 -> CTL = TIMER_A_CTL_TASSEL_2 |
                          TIMER_A_CTL_MC_0 |
                          TIMER_A_CTL_MC__UP |                      //up mode
                          TIMER_A_CTL_CLR;                          //clears timer
}
void initMotoPWM(void)
{
    MOTOR_PORT -> SEL0 |=  MOTOR_PIN;                               // t0 make the pwm signal for the dc motorand turns on
    MOTOR_PORT -> SEL1 &= ~MOTOR_PIN;
    MOTOR_PORT -> DIR  |=  MOTOR_PIN;
}

void setMotoPWM(uint16_t speed)
{
    TIMER_A0 -> CCR[MOTOR_INST] = speed;                            //sets duty cycle
}
void onServoPWM(void)
{
    SERVO_PORT->SEL0 |= SERVO_PIN;                                  //turns servo PWM on
    SERVO_PORT->SEL1 &= ~SERVO_PIN;
    SERVO_PORT->DIR |=   SERVO_PIN;
}
void offServoPWM(void)
{
    SERVO_PORT->SEL0 &= ~SERVO_PIN;                                 //turns servo PWM off
    SERVO_PORT->SEL1 &= ~SERVO_PIN;
}
void setServoOpenPWM(void)
{
    TIMER_A2 -> CCR[SERVO_INST] = OPEN;                             //sets servo PWM to an open duty cycle 90 degrees
}
void setServoClosePWM(void)
{
    TIMER_A2 -> CCR[SERVO_INST] = CLOSE;                            //sets servo PWM to a closed duty cycle 0 degrees
}
