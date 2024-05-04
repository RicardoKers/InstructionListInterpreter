#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VMparameters.h"

// Instructions
#define InstLD 0
#define InstLDN 1
#define InstST 2
#define InstSTN 3
#define InstS 4
#define InstR 5
#define InstMOV 6
#define InstMOVp 7
#define InstAND 8
#define InstANDp 9
#define InstANDN 10
#define InstANDNp 11
#define InstOR 12
#define InstORp 13
#define InstORN 14
#define InstORNp 15
#define InstXOR 16
#define InstXORp 17
#define InstXORN 18
#define InstXORNp 19
#define InstNOT 20
#define InstNOTp 21
#define InstADD 22
#define InstADDp 23
#define InstSUB 24
#define InstSUBp 25
#define InstMUL 26
#define InstMULp 27
#define InstDIV 28
#define InstDIVp 29
#define InstGT 30
#define InstGTp 31
#define InstGE 32
#define InstGEp 33
#define InstEQ 34
#define InstEQp 35
#define InstNE 36
#define InstNEp 37
#define InstLT 38
#define InstLTp 39
#define InstLE 40
#define InstLEp 41
#define InstCTU 42
#define InstCTD 43
#define InstTON 44
#define InstTOF 45
#define Instq 46
#define InstTP 47
#define InstRTRIGGER 48 
#define InstFTRIGGER 49 

// Number of operands
#define NumOpLD 1
#define NumOpLDN 1
#define NumOpST 1
#define NumOpSTN 1
#define NumOpS 1
#define NumOpR 1
#define NumOpMOV 2
#define NumOpMOVp 2
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
#define NumOpNOTp 0
#define NumOpADD 3
#define NumOpADDp 3
#define NumOpSUB 3
#define NumOpSUBp 3
#define NumOpMUL 3
#define NumOpMULp 3
#define NumOpDIV 3
#define NumOpDIVp 3
#define NumOpGT 2
#define NumOpGTp 2
#define NumOpGE 2
#define NumOpGEp 2
#define NumOpEQ 2
#define NumOpEQp 2
#define NumOpNE 2
#define NumOpNEp 2
#define NumOpLT 2
#define NumOpLTp 2
#define NumOpLE 2
#define NumOpLEp 2
#define NumOpCTU 5
#define NumOpCTD 5
#define NumOpTON 5
#define NumOpTOF 5
#define NumOpq 0
#define NumOpTP 5 
#define NumOpRTRIGGER 2 
#define NumOpFTRIGGER 2

// instruction names
const char *InstNames[] = {
  "LD",
  "LDN",
  "ST",
  "STN",
  "S",
  "R",
  "MOV",
  "MOV(",
  "AND",
  "AND(",
  "ANDN",
  "ANDN(",
  "OR",
  "OR(",
  "ORN",
  "ORN(",
  "XOR",
  "XOR(",
  "XORN",
  "XORN(",
  "NOT",
  "NOT(",
  "ADD",
  "ADD(",
  "SUB",
  "SUB(",
  "MUL",
  "MUL(",
  "DIV",
  "DIV(",
  "GT",
  "GT(",
  "GE",
  "GE(",
  "EQ",
  "EQ(",
  "NE",
  "NE(",
  "LT",
  "LT(",
  "LE",
  "LE(",
  "CTU",
  "CTD",
  "TON",
  "TOF",
  ")",
  "TP",
  "RTRIGGER",
  "FTRIGGER" 
};

#define NumInstructions 50

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

#endif