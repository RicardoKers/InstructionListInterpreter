#include "stack.h"
#include <stdio.h>

/*
  Boolean stack
*/

// Initialize the stack
void initStackb(Stackb *s) { s->top = -1; }

// Check if the stack is full
uint8_t isFullb(Stackb *s) { return s->top == STACK_MAX_SIZE - 1; }

// Check if the stack is empty
uint8_t isEmptyb(Stackb *s) { return s->top == -1; }

// Push an element onto the stack
uint8_t pushb(Stackb *s, uint8_t instruction, uint8_t value) {
  if (isFullb(s)) {
    printf("Error: Stack overflow\n");
    return 0;
  }
  s->top++;
  s->elements[s->top].instruction = instruction;
  s->elements[s->top].value = value;
  return 1;
}

// Pop an element from the stack
uint8_t popb(Stackb *s, StackElementb *element) {
  if (isEmptyb(s)) {
    printf("Error: Stack underflow\n");
    return 0;
  }
  *element = s->elements[s->top];
  s->top--;
  return 1;
}



/*
  Integer stack
*/

// Initialize the stack
void initStackw(Stackw *s) { s->top = -1; }

// Check if the stack is full
uint8_t isFullw(Stackw *s) { return s->top == STACK_MAX_SIZE - 1; }

// Check if the stack is empty
uint8_t isEmptyw(Stackw *s) { return s->top == -1; }

// Push an element onto the stack
uint8_t pushw(Stackw *s, uint8_t instruction, int16_t value) {
  if (isFullw(s)) {
    printf("Error: Stack overflow\n");
    return 0;
  }
  s->top++;
  s->elements[s->top].instruction = instruction;
  s->elements[s->top].value = value;
  return 1;
}

// Pop an element from the stack
uint8_t popw(Stackw *s, StackElementw *element) {
  if (isEmptyw(s)) {
    printf("Error: Stack underflow\n");
    return 0;
  }
  *element = s->elements[s->top];
  s->top--;
  return 1;
}
