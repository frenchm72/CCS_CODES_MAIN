#ifndef __SPEAKER_H__
#define __SPEAKER_H__

#define SPEAKER_PORT P5//MACROS FOR THE SPEAKER TO BEEP AND FOR THE PWM
#define SPEAKER_PIN BIT6
#define BEEP_INST 1

#define SMALL_PAUSE 100
#define BIG_PAUSE 500

#define BEEP_PORT P4//TO MAKE A BEEP NOISE PIN
#define BEEP_PIN BIT0

void beep(void);

#endif
