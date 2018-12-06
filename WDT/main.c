#include "msp.h"
#define BLINK_PORT      P1 #define BLINK_PIN       BIT0 #define BLINK_TIME      100
#define BUTTON_PORT     P1 #define BUTTON_PIN      BIT4
void SysTickInit(void); void PinConfig(void);
 uint8_t   blink_flag = 0; uint8_t   timer_flag = 0;
void main(void) {   WDT_A‐>CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer __disable_irq(); PinConfig(); SysTickInit(); NVIC‐>ISER[1] = 0x00000008; //Enable PORT1_IRQn __enable_irq();
 while(1) {     //Foreground Thread } }
void SysTickInit(void) {     SysTick‐>LOAD = 3000*BLINK_TIME;     SysTick‐>VAL = 0;     SysTick‐>CTRL = 0x00000007; } void PinConfig(void) {     // Configure Blink Pin     BLINK_PORT‐>SEL1 &=~ BLINK_PIN;     BLINK_PORT‐>SEL0 &=~ BLINK_PIN;     BLINK_PORT‐>DIR |= BLINK_PIN;     BLINK_PORT‐>OUT &=~ BLINK_PIN;
    //Configure Button Input Pin     BUTTON_PORT‐>SEL0 &=    ~BUTTON_PIN;     BUTTON_PORT‐>SEL1 &=    ~BUTTON_PIN;     BUTTON_PORT‐>DIR &=     ~BUTTON_PIN;     BUTTON_PORT‐>REN |=      BUTTON_PIN;     BUTTON_PORT‐>OUT |=      BUTTON_PIN;     BUTTON_PORT‐>IES |=      BUTTON_PIN;     BUTTON_PORT‐>IE  |=      BUTTON_PIN;     BUTTON_PORT‐>IFG &=     ~BUTTON_PIN;
}
void SysTick_Handler(void) {
    if (blink_flag)  BLINK_PORT‐>OUT ^= BLINK_PIN;     timer_flag = 1; }
void PORT1_IRQHandler(void) {     if(timer_flag && !(BUTTON_PORT‐>IN&BUTTON_PIN))     {         blink_flag = 1;
        WDT_A‐>CTL = 0x5A00 // Watchdog Password           | 1<<5 //Set to ACLK           | 0<<4 //Set to Watchdog mode           | 1<<3 // Clear Timer           | 3; //Set to 2^19 interval (16 seconds)
        timer_flag = 0;     }     BUTTON_PORT‐>IFG &= ~BUTTON_PIN; }
