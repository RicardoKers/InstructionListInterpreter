#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include "VMparameters.h"



/*
Stack structure for boolean values(accumulator)
*/
typedef struct {
  uint8_t instruction;
  uint8_t value;
} StackElementb;

typedef struct {
  StackElementb elements[STACK_MAX_SIZE];
  int top;
} Stackb;

/*
Stack structure for integer values
*/

typedef struct {
  uint8_t instruction;
  int16_t value;
} StackElementw;

typedef struct {
  StackElementw elements[STACK_MAX_SIZE];
  int top;
} Stackw;

extern StackElementb poppedElement;
extern Stackb stackb;

// Function prototypes for boolean stack
void initStackb(Stackb *s);
uint8_t isFullb(Stackb *s);
uint8_t isEmptyb(Stackb *s);
uint8_t pushb(Stackb *s, uint8_t instruction, uint8_t value);
uint8_t popb(Stackb *s, StackElementb *element);

// Function prototypes for integer stack
void initStackw(Stackb *s);
uint8_t isFullw(Stackb *s);
uint8_t isEmptyw(Stackb *s);
uint8_t pushw(Stackw *s, uint8_t instruction, int16_t value);
uint8_t popw(Stackw *s, StackElementw *element);

#endif // STACK_H
