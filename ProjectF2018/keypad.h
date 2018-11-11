#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#define R0_BANK P2                                                          //defines keypins for rows
#define R0_PORT BIT3
#define R1_BANK P6
#define R1_PORT BIT7
#define R2_BANK P6
#define R2_PORT BIT6
#define R3_BANK P5
#define R3_PORT BIT6

#define C0_BANK P3                                                          //defines keypins for columns
#define C0_PORT BIT7
#define C1_BANK P3
#define C1_PORT BIT5
#define C2_BANK P5
#define C2_PORT BIT1

#define COL0_ON        C0_BANK->DIR |=  C0_PORT; C0_BANK->OUT |=  C0_PORT   //to turn on column 0
#define COL0_OFF       C0_BANK->DIR &= ~C0_PORT; C0_BANK->OUT &= ~C0_PORT   //to turn off column 0
#define COL1_ON        C1_BANK->DIR |=  C1_PORT; C1_BANK->OUT |=  C1_PORT   //to turn on column 1
#define COL1_OFF       C1_BANK->DIR &= ~C1_PORT; C1_BANK->OUT &= ~C1_PORT   //to turn off column 1
#define COL2_ON        C2_BANK->DIR |=  C2_PORT; C2_BANK->OUT |=  C2_PORT   //to turn on column 2
#define COL2_OFF       C2_BANK->DIR &= ~C2_PORT; C2_BANK->OUT &= ~C2_PORT   //to turn off column 2

void KeyInit();                                                             //my function prototypes for the key pad
uint16_t readKey(void);
uint8_t readRow(void);
char numTOstring(void);
void getKey(void);
void resetKey(void);

#endif
