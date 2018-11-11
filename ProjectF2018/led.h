#ifndef __LED_H__
#define __LED_H__

#define TA0_PERIOD 7499             //30k tic corresponds to 25ms period 400Hz

#define GREEN_PORT P2               //green LED port
#define GREEN_PIN BIT5              //green LED pin
#define GREEN_INST 2                //green LED instant for the TIMER A

#define BLUE_PORT P2                //blue LED port
#define BLUE_PIN BIT6               //blue LED pin
#define BLUE_INST 3                 //blue LED instant

#define RED_PORT P2                 //red LED port
#define RED_PIN BIT7                //red LED pin
#define RED_INST 4                  //red LED instant

#define DOOR_C_PORT P5              //door closed red LED port
#define DOOR_C_PIN BIT4             //door closed red LED pin

#define DOOR_O_PORT P5              //door open green LED port
#define DOOR_O_PIN BIT5             //door open green LED pin

#define BUT_PORT P3                 //port the button for the lights are on
#define BUT_PIN BIT2                //pin the button for the lights are on

#define MAXBRIGHT 7499              //for the 100% brightness

void butPres(void);                 //function prototypes for LEDs and BUTTONS for LEDs
void initLEDpins(void);
void initTimerALED(void);
void initGreenPWM(void);
void setGreenPWM(uint16_t bright);
void offGreenPWM(void);
void initBluePWM(void);
void setBluePWM(uint16_t bright);
void offBluePWM(void);
void initRedPWM(void);
void setRedPWM(uint16_t bright);
void offRedPWM(void);

#endif
