#include "stack.h"
#include <stdio.h>
// Error codes
#define noError 0 // No error
#define warning 1 // Warning, the execution continues
#define criticalError 2 // Critical error, the execution stops
/*
  Boolean stack
*/

// Initialize the stack
void initStack(Stack *s) { s->top = -1; }

// Check if the stack is full
uint8_t isFull(Stack *s) { return s->top == MAX_SIZE - 1; }

// Check if the stack is empty
uint8_t isEmpty(Stack *s) { return s->top == -1; }

// Push an element onto the stack
uint8_t push(Stack *s, char instruction, uint64_t value) {
  if (isFull(s)) {
    printf("Error: Stack overflow\n");
    return criticalError;
  }
  s->top++;
  s->elements[s->top].instruction = instruction;
  s->elements[s->top].value = value;
  return noError;
}

// Pop an element from the stack
uint8_t pop(Stack *s, StackElement *element) {
  if (isEmpty(s)) {
    printf("Error: Stack underflow\n");
    return 0;
  }
  *element = s->elements[s->top];
  s->top--;
  return noError;
}


