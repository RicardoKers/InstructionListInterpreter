/* Virtual Machine for a PLC (Programmable Logic Controller) using a simple instruction set.

The information storage uses the following nomenclature:
I Boolean inputs;
Q Boolean outputs;
M memories;
    X bit
    B byte 8 bits
    W word 16 bits
    D double word 32 bits
    L long word 64 bits
==============================

Instruction format is as follows:
    6 bits for the instruction
    2 bits for the number of operands
===============================

Operands format is as follows:
    8 bits for the operand type
        3 bits for the Memory type
            0: X bit
            1: B byte 8 bits
            2: W word 16 bits
            3: D double word 32 bits
            4: L long word 64 bits
            5..7: Reserved

        2 bits for the Register type
            0: I input
            1: Q output
            2: M memory
            3: K constant to read a constant value from the program
        
        3 bits for bit number in byte
            0..7: Bit number

    16 bits for the operand address
==============================

The first 2 bytes of the program are the size of the program in bytes.
==============================

The implemented instructions are as follows:
LD (Load): LD source;
LDN (Load Negated): LDN source;
ST (Store): ST destination;
STN (Store Negated): STN destination;
S (Set): S destination;
R (Reset): R destination;
MOV (Move): MOV source, destination;
AND (Logical AND): AND operand;
AND( (Logical AND): AND operand;
OR (Logical OR): OR operand;
OR( (Logical OR): OR operand;
XOR (Logical XOR): XOR operand;
XOR( (Logical XOR): XOR operand;
ANDN (Logical AND Negated): ANDN operand;
ANDN( (Logical AND Negated): ANDN operand;
ORN (Logical OR Negated): ORN operand;
ORN( (Logical OR Negated): ORN operand;
XORN (Logical XOR Negated): XORN operand;
XORN( (Logical XOR Negated): XORN operand;
NOT (Logical NOT): NOT;
NOT( (Logical NOT): NOT;
ADD (Addition): ADD operand1, operand2, destination;
ADD( (Addition): ADD operand1, operand2, destination;
SUB (Subtraction): SUB operand1, operand2, destination;
SUB( (Subtraction): SUB operand1, operand2, destination;
MUL (Multiplication): MUL operand1, operand2, destination;
MUL( (Multiplication): MUL operand1, operand2, destination;
DIV (Division): DIV operand1, operand2, destination;
DIV( (Division): DIV operand1, operand2, destination;
GT (Greater Than): GT operand1, operand2;
GT( (Greater Than): GT operand1, operand2;
GE (Greater or Equal): GE operand1, operand2;
GE( (Greater or Equal): GE operand1, operand2;
EQ (Equal): EQ operand1, operand2;
EQ( (Equal): EQ operand1, operand2;
NE (Not Equal): NE operand1, operand2;
NE( (Not Equal): NE operand1, operand2;
LT (Less Than): LE operand1, operand2;
LT( (Less Than): LE operand1, operand2;
LE (Less or Equal): LE operand1, operand2;
LE( (Less or Equal): LE operand1, operand2;
CTU (Counter Up): CTU operand;
CTD (Counter Down): CTD operand;
TON (Timer On Delay): TON operand;
TOF (Timer Off Delay): TOF operand;
TODO: Criate a stack to support the instructions that use it, and a ) instruction.
*/

#include "VM.h"
/*
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
*/
/**
 * Reads an instruction from a buffer at a given position.
 * 
 * @param buffer The buffer containing the instructions.
 * @param pos The position in the buffer to read the instruction from.
 * @return The instruction read from the buffer.
 */
Instruction readInstruction(uint8_t *buffer, uint16_t *position) {
    Instruction instr;
    uint16_t pos=(*position);
    instr.opcode = (buffer[pos]>>2);
    instr.num_operands = buffer[pos] & 0x03;
    for (uint16_t i = 0; i < instr.num_operands; i++) {
        instr.operands[i].memorytype = buffer[pos+1+(i*3)]>>5;
        instr.operands[i].registertype = (buffer[pos+1+(i*3)]>>3) & 0x03;
        instr.operands[i].bitNumber = buffer[pos+1+(i*3)] & 0x07;
        instr.operands[i].address = (buffer[pos+2+(i*3)]<<8) | buffer[pos+3+(i*3)];
        *position += 3;
    }
    *position += 1;
    return instr;
}

/**
 * Gets a bit from a byte.
 * 
 * @param byte The byte to get the bit from.
 * @param bitNumber The number of the bit to get.
 * @return The value of the bit.
 */
uint8_t getBit(uint8_t byte, uint8_t bitNumber) {
    return (byte >> bitNumber) & 0x01;
}

/**
 * Sets a bit in a byte.
 * 
 * @param byte The byte to set the bit in.
 * @param bitNumber The number of the bit to set.
 * @param value The value to set the bit to.
 * @return The byte with the bit set.
 */
uint8_t setBit(uint8_t byte, uint8_t bitNumber, uint8_t value) {
    if (value == 0) {
        return byte & (uint8_t)(~(1 << bitNumber));
    } else {
        return byte | (uint8_t)(1 << bitNumber);
    }
}

/**
 * Gets a bit from a memory address.
 * 
 * @param memory The memory to get the bit from.
 * @param address The address in the memory to get the bit from.
 * @param bitNumber The number of the bit to get.
 * @return The value of the bit.
*/
uint8_t getBitFormAddress(uint8_t *memory, uint16_t address, uint8_t bitNumber) {
    uint8_t byte = memory[address];
    return getBit(byte, bitNumber);
}

/**
 * Sets a bit in a memory address.
 * 
 * @param memory The memory to set the bit in.
 * @param address The address in the memory to set the bit in.
 * @param bitNumber The number of the bit to set.
 * @param value The value to set the bit to.
 */
void setBitInAddress(uint8_t *memory, uint16_t address, uint8_t bitNumber, uint8_t value) {
    memory[address] = setBit(memory[address], bitNumber, value);
}

/**
 * Gets a Word from a memory address.
 * 
 * @param memory The memory to get the byte from.
 * @param address The address in the memory to get the byte from.
 * @return The value of the byte.
 */
int16_t getWordFromAddress(uint8_t *memory, uint16_t address) {
    return (memory[address] << 8) | memory[address+1];
}

/**
 * Sets a Word in a memory address.
 * 
 * @param memory The memory to set the byte in.
 * @param address The address in the memory to set the byte in.
 * @param value The value to set the byte to.
 */
void setWordInAddress(uint8_t *memory, uint16_t address, int16_t value) {
    memory[address] = (uint8_t)(value >> 8) & 0xFF;
    memory[address+1] = (uint8_t)value & 0xFF;
}

/**
 * Gets a Double Word from a memory address.
 * 
 * @param memory The memory to get the double word from.
 * @param address The address in the memory to get the double word from.
 * @return The value of the double word.
 */
uint32_t getDoubleWordFromAddress(uint8_t *memory, uint16_t address) {
    return ((uint32_t)memory[address] << 24) | ((uint32_t)memory[address+1] << 16) | ((uint32_t)memory[address+2] << 8) | (uint32_t)memory[address+3];
}

/**
 * Sets a Double Word in a memory address.
 * 
 * @param memory The memory to set the double word in.
 * @param address The address in the memory to set the double word in.
 * @param value The value to set the double word to.
 */
void setDoubleWordInAddress(uint8_t *memory, uint16_t address, uint32_t value) {
    memory[address] = (uint8_t)(value >> 24) & 0xFF;
    memory[address+1] = (uint8_t)(value >> 16) & 0xFF;
    memory[address+2] = (uint8_t)(value >> 8) & 0xFF;
    memory[address+3] = (uint8_t)value & 0xFF;
}

/**
 * Gets a Long Word from a memory address.
 * 
 * @param memory The memory to get the long word from.
 * @param address The address in the memory to get the long word from.
 * @return The value of the long word.
 */
uint64_t getLongWordFromAddress(uint8_t *memory, uint16_t address) {
    return ((uint64_t)memory[address] << 56) | ((uint64_t)memory[address+1] << 48) | ((uint64_t)memory[address+2] << 40) | ((uint64_t)memory[address+3] << 32) | ((uint64_t)memory[address+4] << 24) | ((uint64_t)memory[address+5] << 16) | ((uint64_t)memory[address+6] << 8) | (uint64_t)memory[address+7];
}

/**
 * Sets a Long Word in a memory address.
 * 
 * @param memory The memory to set the long word in.
 * @param address The address in the memory to set the long word in.
 * @param value The value to set the long word to.
 */
void setLongWordInAddress(uint8_t *memory, uint16_t address, uint64_t value) {
    memory[address] = (uint8_t)(value >> 56) & 0xFF;
    memory[address+1] = (uint8_t)(value >> 48) & 0xFF;
    memory[address+2] = (uint8_t)(value >> 40) & 0xFF;
    memory[address+3] = (uint8_t)(value >> 32) & 0xFF;
    memory[address+4] = (uint8_t)(value >> 24) & 0xFF;
    memory[address+5] = (uint8_t)(value >> 16) & 0xFF;
    memory[address+6] = (uint8_t)(value >> 8) & 0xFF;
    memory[address+7] = (uint8_t)value & 0xFF;
}

/**
 * Executes an instruction.
 * 
 * @param instr The instruction to execute.
 * @param data The data structure containing the memory and register values.
 */
void executeInstruction(Instruction instr, Data *data)
{
    uint8_t temp=0;
    switch (instr.opcode) {
        case InstLD:
            if(instr.operands[0].memorytype==X) {
                if(instr.operands[0].registertype == I)
                    data->accumulator = getBitFormAddress(data->Inputs, instr.operands[0].address, instr.operands[0].bitNumber);
                else if(instr.operands[0].registertype == Q)
                    data->accumulator = getBitFormAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber);
                else if(instr.operands[0].registertype == M)
                    data->accumulator = getBitFormAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber);
            }
            break;
        case InstLDN:
            if(instr.operands[0].memorytype==X) {
                if(instr.operands[0].registertype == I)
                    data->accumulator = (getBitFormAddress(data->Inputs, instr.operands[0].address, instr.operands[0].bitNumber))==0?1:0;
                else if(instr.operands[0].registertype == Q)
                    data->accumulator = (getBitFormAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber))==0?1:0;
                else if(instr.operands[0].registertype == M)
                    data->accumulator = (getBitFormAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber))==0?1:0;
            }
            break;
        case InstST:
            if(instr.operands[0].memorytype==X) {
                if(instr.operands[0].registertype == Q)
                    setBitInAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber, data->accumulator);
                else if(instr.operands[0].registertype == M)
                    setBitInAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber, data->accumulator);
            }
            break;
        case InstSTN:
            if(instr.operands[0].memorytype==X) {
                if(instr.operands[0].registertype == Q)
                    setBitInAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber, (data->accumulator==0)?1:0);
                else if(instr.operands[0].registertype == M)
                    setBitInAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber, (data->accumulator==0)?1:0);
            }
            break;
        case InstS:
            if(data->accumulator==1) {
                if(instr.operands[0].memorytype==X) {
                    if(instr.operands[0].registertype == Q)
                        setBitInAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber,1);
                    else if(instr.operands[0].registertype == M)
                        setBitInAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber, 1);
                }
            }
            break;
        case InstR:
            if(data->accumulator==1) {
                if(instr.operands[0].memorytype==X) {
                    if(instr.operands[0].registertype == Q)
                        setBitInAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber,0);
                    else if(instr.operands[0].registertype == M)
                        setBitInAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber, 0);
                }
            }
            break;
        case InstMOV:
            if(data->accumulator==1) {
                if(instr.operands[0].memorytype==X) {
                    if(instr.operands[0].registertype == I)
                        temp = getBitFormAddress(data->Inputs, instr.operands[0].address, instr.operands[0].bitNumber);
                    else if(instr.operands[0].registertype == Q)
                        temp = getBitFormAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber);
                    else if(instr.operands[0].registertype == M)
                        temp = getBitFormAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber);
                }
                if(instr.operands[1].memorytype==X) {
                    if(instr.operands[1].registertype == Q)
                        setBitInAddress(data->Outputs, instr.operands[1].address, instr.operands[1].bitNumber, temp);
                    else if(instr.operands[1].registertype == M)
                        setBitInAddress(data->Memories, instr.operands[1].address, instr.operands[1].bitNumber, temp);
                }
                // TODO: add move for other types of memory
            }
            break;            
        case InstMOVp:
            // TODO: add stack support
            break;
        case InstAND:
            if(instr.operands[0].memorytype==X) {
                if(instr.operands[0].registertype == I)
                    data->accumulator = data->accumulator & getBitFormAddress(data->Inputs, instr.operands[0].address, instr.operands[0].bitNumber);
                else if(instr.operands[0].registertype == Q)
                    data->accumulator = data->accumulator & getBitFormAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber);
                else if(instr.operands[0].registertype == M)
                    data->accumulator = data->accumulator & getBitFormAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber);
            }
            break;
        case InstANDp:
            // TODO: add stack support
            break;
        case InstANDN:
            if(instr.operands[0].memorytype==X) {
                if(instr.operands[0].registertype == I)
                    data->accumulator = data->accumulator & (getBitFormAddress(data->Inputs, instr.operands[0].address, instr.operands[0].bitNumber)==0?1:0);
                else if(instr.operands[0].registertype == Q)
                    data->accumulator = data->accumulator & (getBitFormAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber)==0?1:0);
                else if(instr.operands[0].registertype == M)
                    data->accumulator = data->accumulator & (getBitFormAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber)==0?1:0);
            }
            break;
        case InstANDNp:
            // TODO: add stack support
            break;
        case InstOR:
            if(instr.operands[0].memorytype==X) {
                if(instr.operands[0].registertype == I)
                    data->accumulator = data->accumulator | getBitFormAddress(data->Inputs, instr.operands[0].address, instr.operands[0].bitNumber);
                else if(instr.operands[0].registertype == Q)
                    data->accumulator = data->accumulator | getBitFormAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber);
                else if(instr.operands[0].registertype == M)
                    data->accumulator = data->accumulator | getBitFormAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber);
            }
            break;
        case InstORp:
            // TODO: add stack support
            break;
        case InstORN:
            if(instr.operands[0].memorytype==X) {
                if(instr.operands[0].registertype == I)
                    data->accumulator = data->accumulator | (getBitFormAddress(data->Inputs, instr.operands[0].address, instr.operands[0].bitNumber)==0?1:0);
                else if(instr.operands[0].registertype == Q)
                    data->accumulator = data->accumulator | (getBitFormAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber)==0?1:0);
                else if(instr.operands[0].registertype == M)
                    data->accumulator = data->accumulator | (getBitFormAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber)==0?1:0);
            }
            break;
        case InstORNp:
            // TODO: add stack support
            break;
        case InstXOR:
            if(instr.operands[0].memorytype==X) {
                if(instr.operands[0].registertype == I)
                    data->accumulator = data->accumulator ^ getBitFormAddress(data->Inputs, instr.operands[0].address, instr.operands[0].bitNumber);
                else if(instr.operands[0].registertype == Q)
                    data->accumulator = data->accumulator ^ getBitFormAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber);
                else if(instr.operands[0].registertype == M)
                    data->accumulator = data->accumulator ^ getBitFormAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber);
            }
            break;
        case InstXORp:
            // TODO: add stack support
            break;
        case InstXORN:
            if(instr.operands[0].memorytype==X) {
                if(instr.operands[0].registertype == I)
                    data->accumulator = data->accumulator ^ (getBitFormAddress(data->Inputs, instr.operands[0].address, instr.operands[0].bitNumber)==0?1:0);
                else if(instr.operands[0].registertype == Q)
                    data->accumulator = data->accumulator ^ (getBitFormAddress(data->Outputs, instr.operands[0].address, instr.operands[0].bitNumber)==0?1:0);
                else if(instr.operands[0].registertype == M)
                    data->accumulator = data->accumulator ^ (getBitFormAddress(data->Memories, instr.operands[0].address, instr.operands[0].bitNumber)==0?1:0);
            }
            break;
        case InstXORNp:
            // TODO: add stack support
            break;
        case InstNOT:
            data->accumulator = data->accumulator==0?1:0;
            break;
        case InstNOTp:
            // TODO: add stack support
            break;
        // TODO: add more instructions here, ADD, SUB, MUL, DIV, GT, GE, EQ, NE, LT, LE, CTU, CTD, TON, TOF etc
        default:
            break;
    }
}

/**
 * Initializes the memory.
 * 
 * @param data The data structure containing the memory and register values.
 */
void initializeMemory(Data *data) {
    for (uint16_t i = 0; i < MemorySize; i++) {
        data->Memories[i] = 0;
    }
    for (uint16_t i = 0; i < ImputSize; i++) {
        data->Inputs[i] = 0;
    }
    for (uint16_t i = 0; i < OutputSize; i++) {
        data->Outputs[i] = 0;
    }
}

/**
 * Gets the size of the program from a buffer.
 * 
 * @param buffer The buffer containing the program.
 * @return The size of the program.
 */
uint16_t getProgramSize(uint8_t *buffer) {
    return (buffer[0] << 8) | buffer[1];
}

///////////////////////////////////////////////////////////////////////////////////////
// Only for testing
///////////////////////////////////////////////////////////////////////////////////////

/**
 * Encodes an instruction into a buffer.
 * @param buffer The buffer to encode the instruction into.
 * @param bufPos The position in the buffer to encode the instruction at.
 * @param opperation The operation to encode.
 * @param num_operands The number of operands in the instruction.
 * @param operand0 The first operand.
 * @param operand1 The second operand.
 * @param operand2 The third operand.
 * @return The new position in the buffer after encoding the instruction.
*/
uint16_t encodeInstruction(uint8_t *buffer, uint16_t bufPos, uint8_t opperation, uint8_t num_operands, Operand operand0, Operand operand1, Operand operand2) {
    buffer[bufPos] = (opperation << 2) | num_operands;
    bufPos += 1;
    if(num_operands>0){
        buffer[bufPos] = operand0.memorytype<<5 | operand0.registertype<<3 | operand0.bitNumber;
        buffer[bufPos+1] = (uint8_t)(operand0.address >> 8) & 0xFF;
        buffer[bufPos+2] = (uint8_t)operand0.address & 0xFF;
        bufPos += 3;
    }
    if(num_operands>1){
        buffer[bufPos] = operand1.memorytype<<5 | operand1.registertype<<3 | operand1.bitNumber;
        buffer[bufPos+1] = (uint8_t)(operand1.address >> 8) & 0xFF;
        buffer[bufPos+2] = (uint8_t)operand1.address & 0xFF;
        bufPos += 3;
    }
    if(num_operands>2){
        buffer[bufPos] = operand2.memorytype<<5 | operand2.registertype<<3 | operand2.bitNumber;
        buffer[bufPos+1] = (uint8_t)(operand2.address >> 8) & 0xFF;
        buffer[bufPos+2] = (uint8_t)operand2.address & 0xFF;
        bufPos += 3;
    }
    buffer[bufPos] = 0;
    return bufPos;
}

/**
 * Prints an instruction.
 * 
 * @param instr The instruction to print.
 */
void printInstruction(Instruction instr) {
    switch (instr.opcode) {
        case InstLD:
            printf("LD ");
            break;
        case InstLDN:
            printf("LDN ");
            break;
        case InstST:
            printf("ST ");
            break;
        case InstSTN:
            printf("STN ");
            break;
        case InstS:
            printf("S ");
            break;
        case InstR:
            printf("R ");
            break;
        case InstMOV:
            printf("MOV ");
            break;
        case InstMOVp:
            printf("MOV (");
            break;
        case InstAND:
            printf("AND ");
            break;
        case InstANDp:
            printf("AND (");
            break;
        case InstANDN:
            printf("ANDN ");
            break;
        case InstANDNp:
            printf("ANDN (");
            break;
        case InstOR:
            printf("OR ");
            break;
        case InstORp:
            printf("OR (");
            break;
        case InstORN:
            printf("ORN ");
            break;
        case InstORNp:
            printf("ORN (");
            break;
        case InstXOR:
            printf("XOR ");
            break;
        case InstXORp:
            printf("XOR (");
            break;
        case InstXORN:
            printf("XORN ");
            break;
        case InstXORNp:
            printf("XORN (");
            break;
        case InstNOT:
            printf("NOT ");
            break;
        case InstNOTp:
            printf("NOT (");
            break;
        case InstADD:
            printf("ADD ");
            break;
        case InstADDp:
            printf("ADD (");
            break;
        case InstSUB:
            printf("SUB ");
            break;
        case InstSUBp:
            printf("SUB (");
            break;
        case InstMUL:
            printf("MUL ");
            break;
        case InstMULp:
            printf("MUL (");
            break;
        case InstDIV:
            printf("DIV ");
            break;
        case InstDIVp:
            printf("DIV (");
            break;
        case InstGT:
            printf("GT ");
            break;
        case InstGTp:
            printf("GT (");
            break;
        case InstGE:
            printf("GE ");
            break;
        case InstGEp:
            printf("GE (");
            break;
        case InstEQ:
            printf("EQ ");
            break;
        case InstEQp:
            printf("EQ (");
            break;
        case InstNE:
            printf("NE ");
            break;
        case InstNEp:
            printf("NE (");
            break;
        case InstLT:
            printf("LT ");
            break;
        case InstLTp:
            printf("LT (");
            break;
        case InstLE:
            printf("LE ");
            break;
        case InstLEp:
            printf("LE (");
            break;
        case InstCTU:
            printf("CTU ");
            break;
        case InstCTD:
            printf("CTD ");
            break;
        case InstTON:
            printf("TON ");
            break;
        case InstTOF:
            printf("TOF ");
            break;
        default:
            break;
    }
    for (uint16_t i = 0; i < instr.num_operands; i++) {
        if(instr.operands[i].memorytype==X) {
            if(instr.operands[i].registertype == I)
                printf("IX%d.%d ", instr.operands[i].address, instr.operands[i].bitNumber);
            else if(instr.operands[i].registertype == Q)
                printf("QX%d.%d ", instr.operands[i].address, instr.operands[i].bitNumber);
            else if(instr.operands[i].registertype == M)
                printf("MX%d.%d ", instr.operands[i].address, instr.operands[i].bitNumber);
        }
        else if(instr.operands[i].memorytype==B) {
            if(instr.operands[i].registertype == I)
                printf("IB%d ", instr.operands[i].address);
            else if(instr.operands[i].registertype == Q)
                printf("QB%d ", instr.operands[i].address);
            else if(instr.operands[i].registertype == M)
                printf("MB%d ", instr.operands[i].address);
        }
        else if(instr.operands[i].memorytype==W) {
            if(instr.operands[i].registertype == I)
                printf("IW%d ", instr.operands[i].address);
            else if(instr.operands[i].registertype == Q)
                printf("QW%d ", instr.operands[i].address);
            else if(instr.operands[i].registertype == M)
                printf("MW%d ", instr.operands[i].address);
        }
        else if(instr.operands[i].memorytype==D) {
            if(instr.operands[i].registertype == I)
                printf("ID%d ", instr.operands[i].address);
            else if(instr.operands[i].registertype == Q)
                printf("QD%d ", instr.operands[i].address);
            else if(instr.operands[i].registertype == M)
                printf("MD%d ", instr.operands[i].address);
        }
        else if(instr.operands[i].memorytype==L) {
            if(instr.operands[i].registertype == I)
                printf("IL%d ", instr.operands[i].address);
            else if(instr.operands[i].registertype == Q)
                printf("QL%d ", instr.operands[i].address);
            else if(instr.operands[i].registertype == M)
                printf("ML%d ", instr.operands[i].address);
        }
    }
    printf("\n");
}

/**
 * Prints the memory.
 * 
 * @param data The data structure containing the memory and register values.
 */
void printMemory(Data *data) {
        printf("\t");
    for (uint16_t i = 0; i < MemorySize; i++) {
        printf("%d\t",i);
    }
    printf("\nI:\t");
    for (uint16_t i = 0; i < ImputSize; i++) {
        printf("%d\t", data->Inputs[i]);
    }
    printf("\nM:\t");
    for (uint16_t i = 0; i < MemorySize; i++) {
        printf("%d\t", data->Memories[i]);
    }
    printf("\n0:\t");
    for (uint16_t i = 0; i < OutputSize; i++) {
        printf("%d\t", data->Outputs[i]);
    }
    printf("\nAccumulator = %d", data->accumulator);
    printf("\n---------------------------------------------------------------------------------------\n");
}

void printProgramInHEX(uint8_t *program, uint16_t size) {
    printf("\n{");
    for (uint16_t i = 0; i < size; i++) {
        printf("0x%02X", program[i]);
        if(i<size-1)
            printf(",");
    }
    printf("}\n");
}

int main()
{   
    uint8_t program[1000];
    Data data;
    uint16_t bufPos = 2;
    uint16_t programSize = 0;
    initializeMemory(&data);
    // Input test data
    data.Inputs[0] = 0b00000000;
 
    // Test program
    // LD IX0.0
    Operand operand1 = {X, I, 0, 0};
    Operand operand2 = {X, I, 0, 0};
    Operand operand3 = {X, I, 0, 0};
    bufPos=encodeInstruction(program, bufPos, InstLD, 1, operand1, operand2, operand3);
    // AND IX0.1
    operand1 = {X, I, 1, 0};
    bufPos=encodeInstruction(program, bufPos, InstAND, 1, operand1, operand2, operand3);
    // ANDN IX0.2
    operand1 = {X, I, 2, 0};
    bufPos=encodeInstruction(program, bufPos, InstANDN, 1, operand1, operand2, operand3);
    // OR IX0.3
    operand1 = {X, I, 3, 0};
    bufPos=encodeInstruction(program, bufPos, InstOR, 1, operand1, operand2, operand3);
    // ST QX0.0
    operand1 = {X, Q, 0, 0};
    bufPos=encodeInstruction(program, bufPos, InstST, 1, operand1, operand2, operand3);

    programSize = bufPos; // including the 2 bytes for the program size
    program[0] = (uint8_t)(programSize >> 8) & 0xFF;
    program[1] = (uint8_t)programSize & 0xFF;

    // Run the program
    bufPos = 2;
    printMemory(&data);
    while(bufPos < getProgramSize(program)) {
        Instruction instr = readInstruction(program, &bufPos);        
        printInstruction(instr);
        executeInstruction(instr, &data);
        printMemory(&data);
    }   
    printProgramInHEX(program,programSize);
    printf("PSize = %d\n", programSize);
    getchar();
    return 0;
}