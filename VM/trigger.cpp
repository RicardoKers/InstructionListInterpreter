#include "trigger.h"
#include <stdint.h>

void initializeTrigger(Trigger *triggers, uint8_t size){
  for (int aux = 0; (aux < size) && (aux < MAX_TRIGGERS); aux++) {
    triggers[aux]._M = 0;
  }
}
void runRTrigger(Trigger *trigger){
  trigger->QO = trigger->CLK & !(trigger->_M);
  trigger->_M = trigger->CLK;
}
void runFTrigger(Trigger *trigger){
  trigger->QO = !(trigger->CLK) & !(trigger->_M);
  trigger->_M = !trigger->CLK;
}

