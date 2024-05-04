#include <stdint.h>
 
#ifndef COUNTER_H
#define COUNTER_H

#define MAX_COUNTERS 10 // Maximum timers available

/*
Structure for counter
*/
typedef struct {
  uint8_t CO;   // Input CU for counter up or CD for counter down
  uint8_t R_LD; // Input Reset for counter up or LD for dounter down
  uint16_t PV;  // Input
  uint8_t CO_;  // Reserved
  uint8_t QO;   // Output Output Q for counters
  uint16_t CV;  // Output
} Counter;

void initializeCounter(Counter *counters, uint8_t size);

void runCounterUp(Counter *counter);

void runCounterDown(Counter *counter);

#endif