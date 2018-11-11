#include "msp.h"
#include "motor.h"
#include "promain.h"
#include "led.h"
#include <stdio.h>
#include <stdlib.h>

void initLEDpins(void)
{
    GREEN_PORT -> SEL0 &= ~GREEN_PIN;                       //sets up green LED pin
    GREEN_PORT -> SEL1 &= ~GREEN_PIN;
    GREEN_PORT -> DIR  |=  GREEN_PIN;
    GREEN_PORT -> OUT  &= ~GREEN_PIN;

    BLUE_PORT -> SEL0 &= ~BLUE_PIN;                         //sets up blue LED pin
    BLUE_PORT -> SEL1 &= ~BLUE_PIN;
    BLUE_PORT -> DIR  |=  BLUE_PIN;
    BLUE_PORT -> OUT  &= ~BLUE_PIN;

    RED_PORT -> SEL0 &= ~RED_PIN;                           //sets up red LED pin
    RED_PORT -> SEL1 &= ~RED_PIN;
    RED_PORT -> DIR  |=  RED_PIN;
    RED_PORT -> OUT  &= ~RED_PIN;

    DOOR_C_PORT -> SEL0 &= ~DOOR_C_PIN;                     //sets up door red LED for closed
    DOOR_C_PORT -> SEL1 &= ~DOOR_C_PIN;
    DOOR_C_PORT -> DIR  |=  DOOR_C_PIN;
    DOOR_C_PORT -> OUT  |=  DOOR_C_PIN;

    DOOR_O_PORT -> SEL0 &= ~DOOR_O_PIN;                     //set up door green LED for open
    DOOR_O_PORT -> SEL1 &= ~DOOR_O_PIN;
    DOOR_O_PORT -> DIR  |=  DOOR_O_PIN;
    DOOR_O_PORT -> OUT  &= ~DOOR_O_PIN;

    BUT_PORT->SEL0 &= ~BUT_PIN;                             //setting up button for the LEDS
    BUT_PORT->SEL1 &= ~BUT_PIN;
    BUT_PORT->DIR |= ~0xFF;
    BUT_PORT->REN |= BUT_PIN;
    BUT_PORT->OUT |= BUT_PIN;
}

void initTimerALED(void)
{
    TIMER_A0 -> CCR[GREEN_INST] = 0;                        //sets up timer A0 for the PWM signal for the LEDS to used in different instances
    TIMER_A0 -> CCTL[GREEN_INST] = TIMER_A_CCTLN_OUTMOD_7;

    TIMER_A0 -> CCR[BLUE_INST] = 0;
    TIMER_A0 -> CCTL[BLUE_INST] = TIMER_A_CCTLN_OUTMOD_7;

    TIMER_A0 -> CCR[RED_INST] = 0;
    TIMER_A0 -> CCTL[RED_INST] = TIMER_A_CCTLN_OUTMOD_7;

    TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_0
            | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;
}

void initGreenPWM(void)
{
    GREEN_PORT -> OUT  |=  GREEN_PIN;
    GREEN_PORT -> SEL0 |=  GREEN_PIN;                       // t0 make the green pwm signal turn on
    GREEN_PORT -> SEL1 &= ~GREEN_PIN;
    GREEN_PORT -> DIR  |=  GREEN_PIN;
}

void setGreenPWM(uint16_t bright)
{
    TIMER_A0 -> CCR[GREEN_INST] = bright;                   //sets the duty cycle of green
}
void offGreenPWM(void)
{
    GREEN_PORT -> OUT  &= ~GREEN_PIN;
    GREEN_PORT -> SEL0 &= ~GREEN_PIN;                       // t0 make the green pwm signal turn off
    GREEN_PORT -> SEL1 &= ~GREEN_PIN;
}


void initBluePWM(void)
{
    BLUE_PORT -> OUT  |=  BLUE_PIN;
    BLUE_PORT -> SEL0 |=  BLUE_PIN;                         // t0 make the pwm signal turn on
    BLUE_PORT -> SEL1 &= ~BLUE_PIN;
    BLUE_PORT -> DIR  |=  BLUE_PIN;
}

void setBluePWM(uint16_t bright)
{
    TIMER_A0 -> CCR[BLUE_INST] = bright;                    //sets duty cycle for the blue LED
}
void offBluePWM(void)
{
    BLUE_PORT -> OUT  &= ~BLUE_PIN;
    BLUE_PORT -> SEL0 &= ~BLUE_PIN;                         // t0 make the pwm signal turn off
    BLUE_PORT -> SEL1 &= ~BLUE_PIN;
}

void initRedPWM(void)
{
    RED_PORT -> OUT  |=  RED_PIN;
    RED_PORT -> SEL0 |=  RED_PIN;                           // t0 make the pwm signal turn on
    RED_PORT -> SEL1 &= ~RED_PIN;
    RED_PORT -> DIR  |=  RED_PIN;
}

void setRedPWM(uint16_t bright)
{
    TIMER_A0 -> CCR[RED_INST] = bright;                     //sets the duty cycle of the RED LED
}
void offRedPWM(void)
{
    RED_PORT -> OUT  &= ~RED_PIN;
    RED_PORT -> SEL0 &= ~RED_PIN;                           // t0 make the pwm signal turn off
    RED_PORT -> SEL1 &= ~RED_PIN;
}
