#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VMparameters.h"
#include "trigger.h"
#include "stack.h"
#include "timer.h"
#include "counter.h"

// Instructions
#define InstLD 0
#define InstLDN 1
#define InstST 2
#define InstSTN 3
#define InstS 4
#define InstR 5
#define InstMOV 6
#define InstAND 7
#define InstANDp 8
#define InstANDN 9
#define InstANDNp 10
#define InstOR 11
#define InstORp 12
#define InstORN 13
#define InstORNp 14
#define InstXOR 15
#define InstXORp 16
#define InstXORN 17
#define InstXORNp 18
#define InstNOT 19
#define InstADD 20
#define InstSUB 21
#define InstMUL 22
#define InstDIV 23
#define InstMOD 24
#define InstGT 25
#define InstGE 26
#define InstEQ 27
#define InstNE 28
#define InstLT 29
#define InstLE 30
#define InstCTU 31
#define InstCTD 32
#define InstTON 33
#define InstTOF 34
#define Instq 35
#define InstTP 36
#define InstRTRIGGER 37 
#define InstFTRIGGER 38 

// Number of operands
#define NumOpLD 1
#define NumOpLDN 1
#define NumOpST 1
#define NumOpSTN 1
#define NumOpS 1
#define NumOpR 1
#define NumOpMOV 2
#define NumOpAND 1
#define NumOpANDp 1
#define NumOpANDN 1
#define NumOpANDNp 1
#define NumOpOR 1
#define NumOpORp 1
#define NumOpORN 1
#define NumOpORNp 1
#define NumOpXOR 1
#define NumOpXORp 1
#define NumOpXORN 1
#define NumOpXORNp 1
#define NumOpNOT 0
#define NumOpADD 3
#define NumOpSUB 3
#define NumOpMUL 3
#define NumOpDIV 3
#define NumOpMOD 3
#define NumOpGT 2
#define NumOpGE 2
#define NumOpEQ 2
#define NumOpNE 2
#define NumOpLT 2
#define NumOpLE 2
#define NumOpCTU 6
#define NumOpCTD 6
#define NumOpTON 6
#define NumOpTOF 6
#define NumOpq 0
#define NumOpTP 5 
#define NumOpRTRIGGER 3 
#define NumOpFTRIGGER 3

// Memory types
#define X 0 // Bit
#define B 1 // Byte 8 bits
#define W 2 // Word 16 bits
#define D 3 // Double word 32 bits
#define L 4 // Long word 64 bits
#define R 5 // Real (float) 32 bits

// Register types
#define I 0 // Input
#define Q 1 // Output
#define M 2 // Memory
#define K 3 // Constant from the program

// Error codes
#define noError 0 // No error
#define warning 1 // Warning, the execution continues
#define criticalError 2 // Critical error, the execution stops

// Isntruction definition
#define MaxOpers 6

// Data structure
typedef struct stData {
  // Memory variables
  uint8_t Memories[MemorySize]; // Memories in bytes
  uint8_t Inputs[InputSize];    // Inputs in bytes
  uint8_t Outputs[OutputSize];  // Outputs in bytes
  uint8_t accumulator;
} Data;

typedef struct stOperand {
  uint8_t memorytype;
  uint8_t registertype;
  uint8_t bitNumber;
  uint16_t address; // Or constant value if type is K
} Operand;

typedef struct stInstruction {
  uint8_t opcode;
  uint8_t num_operands;
  Operand operands[MaxOpers];
} Instruction;

// Union to convert data types: uint8, uint16, uint32, uint64, int8, int16, int32, int64
typedef union {
  uint8_t *u8;
  uint16_t *u16;
  uint32_t *u32;
  uint64_t *u64;
  int8_t *i8;
  int16_t *i16;
  int32_t *i32;
  int64_t *i64;
  float *f;
} DataUnion;

// Function prototypes
uint8_t getNumOp(uint8_t inst);
void initializeMemory(Data *data, Timer *atimers, Counter *acounters, Trigger *atriggers, Stack *astack);
void executeInstruction(uint8_t *buffer, Instruction instr, Data *data);
Instruction readInstruction(uint8_t *buffer, uint16_t *position);
uint16_t getProgramSize(uint8_t *buffer);
uint8_t verifyProgramIntegrity(uint8_t *buffer);
int8_t operandValueToInt8(Operand *oper, uint8_t *program, Data *data);
int16_t operandValueToInt16(Operand *oper, uint8_t *program, Data *data);
void setWordInAddress(uint8_t *memory, uint16_t address, int16_t value);
void setDoubleWordInAddress(uint8_t *memory, uint16_t address, uint32_t value);
void setLongWordInAddress(uint8_t *memory, uint16_t address, uint64_t value);
int16_t getWordFromAddress(uint8_t *memory, uint16_t address);
int32_t getDoubleWordFromAddress(uint8_t *memory, uint16_t address);
int64_t getLongWordFromAddress(uint8_t *memory, uint16_t address);
float getFloatFromAddress(uint8_t *memory, uint16_t address);
#endif