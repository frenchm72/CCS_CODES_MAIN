#ifndef __MOTOR_H__
#define __MOTOR_H__

#define TA0_PERIOD 7499         //30k tic corresponds to 25ms period 400Hz

#define MOTOR_PORT P2           //motor port
#define MOTOR_PIN BIT4          //motor pin
#define MOTOR_INST 1            //inst for motor

/*#define MOTOR_POSITION_PORT P6  //servo position port
#define MOTOR_POSITION_PIN BIT1 //servo position pin*/

#define MOTOSPD 7499            //30k tic corresponds to 25ms period 400Hz

#define SERVO_PORT P5           //servo port
#define SERVO_PIN BIT7          //servo pin
#define SERVO_INST 2            //servo instant

#define SERVO_PERIOD 20000      //period for the servo

#define CLOSE 3000              //closed duty cycle
#define OPEN  1010              //open duty cycle

void initMOTOpins(void);        //init for the for all motor pins
void initTimerAmoto(void);      //init for the timer a for the motor and servo
void initMotoPWM(void);         //init the motor PWM
void setMotoPWM(uint16_t spee); //sets the duty cycle
void onServoPWM(void);          //servo pwm init
void offServoPWM(void);         //turns the servo pwm off
void setServoOpenPWM(void);     //to set the duty cycle to open
void setServoClosePWM(void);    //to set the duty cycle to close

#endif
