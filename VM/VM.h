#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


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

// Memory definition
#define MemorySize 10 // Size of the memory in bytes
#define ImputSize 10 // Number of inputs in bytes
#define OutputSize 10 // Number of outputs in bytes

#define X 0 // Bit
#define B 1 // Byte 8 bits
#define W 2 // Word 16 bits
#define D 3 // Double word 32 bits
#define L 4 // Long word 64 bits

#define I 0 // Input
#define Q 1 // Output
#define M 2 // Memory
#define K 3 // Constant from the program

typedef struct stData {
	// Memory variables
	uint8_t Memories[MemorySize]; // Memories in bytes
	uint8_t Inputs[ImputSize]; // Inputs in bytes
	uint8_t Outputs[OutputSize]; // Outputs in bytes
	uint8_t accumulator;
} Data;

typedef struct stOperand {
	uint8_t memorytype;
	uint8_t registertype;
	uint8_t bitNumber;
	uint16_t address; // Or constant value if type is K
}Operand;

typedef struct stInstruction {
	uint8_t opcode;
	uint8_t num_operands;
	Operand operands[3];
} Instruction;

void initializeMemory(Data *data);
void executeInstruction(Instruction instr, Data *data);
Instruction readInstruction(uint8_t *buffer, uint16_t *position);
uint16_t getProgramSize(uint8_t *buffer);