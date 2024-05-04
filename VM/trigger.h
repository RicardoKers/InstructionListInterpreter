#ifndef TRIGGER_H
#define TRIGGER_H

#include <stdint.h>

#define MAX_TRIGGERS 10 // Maximum triggers available

typedef struct {
  uint8_t CLK;         // Input
  uint8_t _M;         // Reserved
  uint8_t QO;         // Ouput
} Trigger;

void initializeTrigger(Trigger *triggers, uint8_t size);
void runRTrigger(Trigger *trigger);
void runFTrigger(Trigger *trigger);
#endif