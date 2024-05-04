/*
Struct is defined has 32bit values beacuse considering a tick of 1ms
32bit allows counting up to 49 days.
16bit allows a maximum of 655 seconds

Timer definition. Each timer is a struct with the followingvariables:
IN -> Input of timer
PT -> Preset os Tick number
EN -> Internal use to define when timer started
INITTICKS -> Ticks count when timer started countign
QO -> Output
ET -> Elapsed Ticks

*/

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define MAX_TIMERS 10 // Maximum timers available

extern volatile uint32_t ElapsedTicks;

/*
Structure for timers
Not Thread Safe
*/
typedef struct {
  uint8_t IN;         // Input
  uint16_t PT;        // Input
  uint8_t prescaler;  // Input
  uint8_t EN;         // Reserved
  uint32_t InitTicks; // Reserved
  uint8_t state;      // Reserved
  uint8_t QO;         // Ouput
  uint16_t ET;        // Ouput
} Timer;

void initializeTimer(Timer *timers, uint8_t size);
void updateTicks(uint8_t nticks);
void runTimerTON(Timer *timer);
void runTimerTOF(Timer *timer);
void runTimerTP(Timer *timer);

#endif