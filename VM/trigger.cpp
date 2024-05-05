#include "trigger.h"
#include <stdint.h>
#include <stdio.h>

void initializeTrigger(Trigger *triggers, uint8_t size){
  for (int aux = 0; (aux < size) && (aux < MAX_TRIGGERS); aux++) {
    triggers[aux]._M = 0;
  }
}
void runRTrigger(Trigger *trigger){
  trigger->QO = trigger->CLK & !(trigger->_M);
  trigger->_M = trigger->CLK;
  printf("clk:%d\n",trigger->CLK);
  printf("qo:%d\n",trigger->QO);
  printf("_m:%d\n",trigger->_M);
}
void runFTrigger(Trigger *trigger){
  trigger->QO = !(trigger->CLK) & !(trigger->_M);
  trigger->_M = !trigger->CLK;
}

