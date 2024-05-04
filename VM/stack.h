#ifndef STACK_H
#define STACK_H


#include <stdint.h>

#define MAX_SIZE 10 // Maximum stack size

/*
Stack structure for boolean values(accumulator)
*/
typedef struct {
  uint8_t instruction;
  uint8_t value;
} StackElementb;

typedef struct {
  StackElementb elements[MAX_SIZE];
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
  StackElementw elements[MAX_SIZE];
  int top;
} Stackw;


// Function prototypes for boolean stack
void initStackb(Stackb *s);
uint8_t isFullb(Stackb *s);
uint8_t isEmptyb(Stackb *s);
uint8_t pushb(Stackb *s, char instruction, uint8_t value);
uint8_t popb(Stackb *s, StackElementb *element);

// Function prototypes for integer stack
void initStackw(Stackb *s);
uint8_t isFullw(Stackb *s);
uint8_t isEmptyw(Stackb *s);
uint8_t pushw(Stackw *s, char instruction, int16_t value);
uint8_t popw(Stackw *s, StackElementw *element);



#endif // STACK_H
