#include "counter.h"

#include <stdio.h>

void initializeCounter(Counter counters[], uint8_t size) {
  for (int aux = 0; (aux < size) && (aux < MAX_COUNTERS); aux++) {
    counters[aux].CO = 0;
    counters[aux].R_LD = 0;
    counters[aux].PV = 0;
    counters[aux].CO_ = 0;
    counters[aux].QO = 0;
    counters[aux].CV = 0;
  }
}

void runCounterUp(Counter *counter) {
  if (counter->R_LD == 1) {
    counter->CV = 0;
  } else if (counter->CO == 1 && counter->CO_ == 0) {
    counter->CO_ = 1;
    if (counter->CV < counter->PV) {
      counter->CV++;
    }
  } else if (counter->CO == 0 && counter->CO_ == 1) {
    counter->CO_ = 0;
  }
  counter->QO = (counter->CV >= counter->PV) ? 1 : 0;

}

void runCounterDown(Counter *counter) {
  if (counter->R_LD == 1) {
    counter->CV = counter->PV;
  } else if (counter->CO == 1 && counter->CO_ == 0) {
    counter->CO_ = 1;
    if (counter->CV > 0) {
      counter->CV--;
    }
  } else if (counter->CO == 0 && counter->CO_ == 1) {
    counter->CO_ = 0;
  }
  counter->QO = (counter->CV <= 0) ? 1 : 0;
  printf("CV:%d\n", counter->CV);
  printf("PV:%d\n", counter->PV);
  printf("Qo:%d\n", counter->QO);
  printf("IN:%d\n", counter->CO);
  
}