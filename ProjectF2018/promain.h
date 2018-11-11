#ifndef __PROMAIN_H__
#define __PROMAIN_H__

#define TIMEOUT 200                    //20 is ~1 second 1200 is ~1 min in cycles of our program

#define BUTTON_PORT P1                  //emergency button port
#define BUTTON_PIN  BIT5                //emergency button pin
#define BUTTON_IN   PORT1_IRQHandler    //port handler for emergency button

void StsTick_Init(void);                //these function are used throughout the program where other headers didn't have
void delay_micro(int mic);
void delay_ms(int ms);

void convertINTmoto(void);              //functions to convert the strings into functions
void convertINTred(void);
void convertINTblue(void);
void convertINTgreen(void);

#endif
