#ifndef __KEYPAD_H__
#define __KEYPAD_H__


//KEY PIN DEFINITIONS
#define R0_BANK P4
#define R0_PORT BIT5
#define R1_BANK P4
#define R1_PORT BIT4
#define R2_BANK P4
#define R2_PORT BIT2
#define R3_BANK P4
#define R3_PORT BIT0

#define C0_BANK P5
#define C0_PORT BIT5
#define C1_BANK P5
#define C1_PORT BIT4
#define C2_BANK P4
#define C2_PORT BIT7

#define COL0_ON        C0_BANK->DIR |=  C0_PORT; C0_BANK->OUT |=  C0_PORT
#define COL0_OFF       C0_BANK->DIR &= ~C0_PORT; C0_BANK->OUT &= ~C0_PORT
#define COL1_ON        C1_BANK->DIR |=  C1_PORT; C1_BANK->OUT |=  C1_PORT
#define COL1_OFF       C1_BANK->DIR &= ~C1_PORT; C1_BANK->OUT &= ~C1_PORT
#define COL2_ON        C2_BANK->DIR |=  C2_PORT; C2_BANK->OUT |=  C2_PORT
#define COL2_OFF       C2_BANK->DIR &= ~C2_PORT; C2_BANK->OUT &= ~C2_PORT


//FUNCTIONS
void KeyInit();
uint16_t readKey(void);
uint8_t readRow(void);
int _delay_ms(int ms);
void printNum(uint16_t);
#endif
