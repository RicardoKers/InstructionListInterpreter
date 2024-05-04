#include "timer.h"
//#include <stdio.h>
volatile uint32_t ElapsedTicks = 0;

void initializeTimer(Timer timers[], uint8_t size) {
  for (int aux = 0; (aux < size) && (aux < MAX_TIMERS); aux++) {
    timers[aux].EN = 0;
    timers[aux].ET = 0;
    timers[aux].IN = 0;
    timers[aux].InitTicks = 0;
    timers[aux].PT = 0;
    timers[aux].QO = 0;
    timers[aux].prescaler = 1;
    timers[aux].state = 0;
  }
}

void updateTicks(uint8_t nticks) { ElapsedTicks += nticks; }

void runTimerTOF(Timer *timer) {
  if (timer->IN == 1) {
    timer->QO = 1;
    timer->ET = 0;
    timer->EN = 0;
    timer->InitTicks = 0;
  } else {
    if (timer->EN == 0 && timer->QO == 1) {
      timer->EN = 1;
      timer->InitTicks = ElapsedTicks;
    }
    if (timer->EN == 1) {
      timer->ET = (uint16_t)((ElapsedTicks - timer->InitTicks)/ (uint32_t)timer->prescaler);
      if (timer->ET >= timer->PT) {
        timer->ET = 0;
        timer->QO = 0;
        timer->EN = 0;
      } else {
        timer->QO = 1;
      }
    }
  }
}

void runTimerTON(Timer *timer) {
  //printf("TotalTicks:%ld\n", (long)ElapsedTicks);
  if (timer->IN == 0) {
    timer->QO = 0;
    timer->ET = 0;
    timer->EN = 0;
    timer->InitTicks = 0;
  } else {
    if (timer->EN == 0 && timer->QO == 0) {
      timer->EN = 1;
      timer->InitTicks = ElapsedTicks ;
    }
    if (timer->EN == 1) {
      if (timer->ET < timer->PT) {
        timer->ET = (uint16_t)((ElapsedTicks - timer->InitTicks)/ (uint32_t)timer->prescaler);
      }
      if (timer->ET >= timer->PT) {
        timer->QO = 1;
        timer->ET = timer->PT;
      } else {
        timer->QO = 0;
      }
    }
  }
  /*
  printf("ITicks:%d\n", timer->InitTicks);
  printf("PT:%d\n", timer->PT);
  printf("ET:%d\n", timer->ET);
  printf("Qo:%d\n", timer->QO);
  printf("IN:%d\n", timer->IN);
  printf("Sta:%d\n", timer->state);
  updateTicks(4);
  */
}

// State 0 -> timer off, output off, input off
// State 1 -> timer on, output on, input on
// State 2 -> timer off, output off, input xx

void runTimerTP(Timer *timer) {
  if (timer->state == 0 && timer->IN == 1) {
    timer->state = 1;
    timer->EN = 1;
    timer->InitTicks = ElapsedTicks;
    timer->QO = 1;
  } else if (timer->state == 1) {
    timer->ET = (uint16_t)((ElapsedTicks - timer->InitTicks)/ (uint32_t)timer->prescaler);
    if (timer->ET >= timer->PT) {
      timer->QO = 0;
      timer->ET = 0;
      timer->EN = 0;
      if (timer->IN == 0)
        timer->state = 0;
      else if (timer->IN == 1)
        timer->state = 2;
    } else {
      timer->QO = 1;
    }
  } else if (timer->state == 2) {
    if (timer->IN == 0) {
      timer->state = 0;
    }
  }

}
