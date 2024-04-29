#ifndef TIMER_H
#include "timer.h"
#endif

uint32_t ElapsedTicks = 0;
TimerTON timers_ton[MAX_TEMPS];

void updateTicks() { ElapsedTicks++; }

void runTimerTON(uint8_t timerNumber) {
  if (timers_ton[timerNumber].IN == 0) {
    timers_ton[timerNumber].Qo = 0;
    timers_ton[timerNumber].ET = 0;
    timers_ton[timerNumber].EN = 0;
    timers_ton[timerNumber].InitTicks = 0;
  } else {
    if (timers_ton[timerNumber].EN == 0) {
      timers_ton[timerNumber].EN = 1;
      timers_ton[timerNumber].InitTicks = ElapsedTicks;
    }
    timers_ton[timerNumber].ET =
        ElapsedTicks - timers_ton[timerNumber].InitTicks;
    if (timers_ton[timerNumber].ET > timers_ton[timerNumber].PT) {
      timers_ton[timerNumber].Qo = 1;
    } else {
      timers_ton[timerNumber].Qo = 0;
    }
  }
}

