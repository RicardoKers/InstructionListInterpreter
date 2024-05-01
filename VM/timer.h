#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "VMparameters.h"

extern uint32_t ElapsedTicks;

/*
Structure for timers
Still not Thread Safe...
*/
typedef struct {
  uint8_t IN;         // Input
  uint16_t PT;        // Input
  uint8_t EN;         // Internal
  uint32_t InitTicks; // Internal
  uint8_t Qo;         // Ouput
  uint32_t ET;        // Ouput
} TimerTON;

extern TimerTON timers_ton[MAX_TEMPS];

//void updateTicks();
//void runTimerTON(uint8_t timerNumber);

#endif
