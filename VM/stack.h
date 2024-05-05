#ifndef STACK_H
#define STACK_H


#include <stdint.h>

#define MAX_SIZE 10 // Maximum stack size

/*
Stack structure for boolean values(accumulator)
*/
typedef struct {
  uint8_t instruction;
  uint64_t value;
} StackElement;

typedef struct {
  StackElement elements[MAX_SIZE];
  int top;
} Stack;

// Function prototypes for boolean stack
void initStack(Stack *s);
uint8_t isFull(Stack *s);
uint8_t isEmpty(Stack *s);
uint8_t push(Stack *s, char instruction, uint64_t value);
uint8_t pop(Stack *s, StackElement *element);



#endif // STACK_H
