/* Virtual Machine for a PLC (Programmable Logic Controller) using a simple
instruction set.

The information storage uses the following nomenclature:
I Boolean inputs;
Q Boolean outputs;
M memories;
    X bit
    B byte 8 bits
    W word 16 bits
    D double word 32 bits
    L long word 64 bits
    R real word 32 bits (float point)
==============================

Instruction format is as follows:
    8 bits for the instruction
===============================

Operands format is as follows:
    8 bits for the operand type
        3 bits for the Memory type
            0: X bit
            1: B byte 8 bits
            2: W word 16 bits
            3: D double word 32 bits
            4: L long word 64 bits
            5: R real word 32 bits (float point)
            6..7: Reserved

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
0 LD (Load): LD source;
1 LDN (Load Negated): LDN source;
2 ST (Store): ST destination;
3 STN (Store Negated): STN destination;
4 S (Set): S destination;
5 R (Reset): R destination;
6 MOV (Move): MOV source, destination;
7 AND (Logical AND): AND operand;
8 AND( (Logical AND, open parentheses): AND( operand;
9 ANDN (Logical AND Negated): ANDN operand;
10 ANDN( (Logical AND Negated, open parentheses): ANDN( operand;
11 OR (Logical OR): OR operand;
12 OR( (Logical OR, open parentheses): OR( operand;
13 ORN (Logical OR Negated): ORN operand;
14 ORN( (Logical OR Negated, open parentheses): ORN( operand;
15 XOR (Logical XOR): XOR operand;
16 XOR( (Logical XOR, open parentheses): XOR( operand;
17 XORN (Logical XOR Negated): XORN operand;
18 XORN( (Logical XOR Negated, open parentheses): XORN( operand;
19 NOT (Logical NOT): NOT;
20 ADD (Addition): ADD operand1, operand2, destination;
21 SUB (Subtraction): SUB operand1, operand2, destination;
22 MUL (Multiplication): MUL operand1, operand2, destination;
23 DIV (Division): DIV operand1, operand2, destination;
24 MOD (Remainder): MOD operand1, operand2, destination;
25 GT (Greater Than): GT operand1, operand2;
26 GE (Greater or Equal): GE operand1, operand2;
27 EQ (Equal): EQ operand1, operand2;
28 NE (Not Equal): NE operand1, operand2;
29 LT (Less Than): LE operand1, operand2;
30 LE (Less or Equal): LE operand1, operand2;
31 CTU (Counter Up): CTU(ncouter, CO, PV, RST, OUT); //aqui
32 CTD (Counter Down): CTD(ncouter, CO, PV, LD, OUT);
33 TON (Timer On Delay): TON(ntimer, IN, PT, prescaler, OUT); Example TON(K5, IX0.0, 10,1,QX0.1) //aqui
34 TOF (Timer Off Delay): TOF operand;
35 ) (close parentheses): ); "Stract instruction from stack";
36 TP (Timer Pulse);
37 R_TRIGGER (Rising edge detection) R_TRIGGER (ntrigger,IN, QO);
38 F_TRIGGER (Falling edge detection) F_TRIGGER (ntrigger,IN, QO);
*/

#include "VM.h"
#include "stack.h"
#include "timer.h"
#include "counter.h"
#include "trigger.h"
#include <stdio.h>
StackElement poppedElement;
Stack *stack;
Timer *timers;
Counter *counters;
Trigger *triggers;


/**
 * Gets the number of operands for an instruction.
 *
 * @param inst The instruction to get the number of operands for. *
 */
uint8_t getNumOp(uint8_t inst) {
  switch (inst)
  {
  case InstLD:
      return NumOpLD;
    break;
  case InstLDN:
      return NumOpLDN;
    break;
  case InstST:
      return NumOpST;
    break;
  case InstSTN:
      return NumOpSTN;
    break;
  case InstS:
      return NumOpS;
    break;
  case InstR:
      return NumOpR;
    break;
  case InstMOV:
      return NumOpMOV;
    break;
  case InstAND:
      return NumOpAND;
    break;
  case InstANDp:
      return NumOpANDp;
    break;
  case InstANDN:
      return NumOpANDN;
    break;
  case InstANDNp:
      return NumOpANDNp;
    break;
  case InstOR:
      return NumOpOR;
    break;
  case InstORp:
      return NumOpORp;
    break;
  case InstORN:
      return NumOpORN;
    break;
  case InstORNp:
      return NumOpORNp;
    break;
  case InstXOR:
      return NumOpXOR;
    break;
  case InstXORp:
      return NumOpXORp;
    break;
  case InstXORN:
      return NumOpXORN;
    break;
  case InstXORNp:
      return NumOpXORNp;
    break;
  case InstNOT:
      return NumOpNOT;
    break;
  case InstADD:
      return NumOpADD;
    break;
  case InstSUB:
      return NumOpSUB;
    break;
  case InstMUL:
      return NumOpMUL;
    break;
  case InstDIV:
      return NumOpDIV;
    break;
  case InstMOD:
      return NumOpMOD;
    break;
  case InstGT:
      return NumOpGT;
    break;
  case InstGE:
      return NumOpGE;
    break;
  case InstEQ:
      return NumOpEQ;
    break;
  case InstNE:
      return NumOpNE;
    break;
  case InstLT:
      return NumOpLT;
    break;
  case InstLE:
      return NumOpLE;
    break;
  case InstCTU:
      return NumOpCTU;
    break;
  case InstCTD:
      return NumOpCTD;
    break;
  case InstTON:
      return NumOpTON;
    break;
  case InstTOF:
      return NumOpTOF;
    break;
  case InstTP: //aqui
    return NumOpTP;
    break;
  case InstRTRIGGER://aqui
    return NumOpRTRIGGER;
    break;
  case InstFTRIGGER://aqui
    return NumOpFTRIGGER;
    break;
  case Instq:
      return NumOpq;
  default:
      return 0;
    break;
  }
  return 0;
}

/**
 * Reads an instruction from a buffer at a given position.
 *
 * @param buffer The buffer containing the instructions.
 * @param pos The position in the buffer to read the instruction from.
 * @return The instruction read from the buffer.
 */
Instruction readInstruction(uint8_t *buffer, uint16_t *position) {
  Instruction instr;
  uint16_t pos = (*position);
  instr.opcode = buffer[pos];
  instr.num_operands = getNumOp(instr.opcode);
  pos++;
  for (uint16_t i = 0; i < instr.num_operands; i++) {
    instr.operands[i].memorytype = buffer[pos] >> 5;
    instr.operands[i].registertype = (buffer[pos] >> 3) & 0x03;
    instr.operands[i].bitNumber = buffer[pos] & 0x07;
    pos++;
    if(instr.operands[i].registertype != K)
    {
      instr.operands[i].address = (buffer[pos] << 8) | buffer[pos + 1];
      pos += 2;
    }
    else
    {
      instr.operands[i].address = pos;
      if(instr.operands[i].memorytype == X)
        pos += 1;
      if(instr.operands[i].memorytype == B)
        pos += 1;
      if(instr.operands[i].memorytype == W)
        pos += 2;
      if(instr.operands[i].memorytype == D)
        pos += 4;
      if(instr.operands[i].memorytype == L)
        pos += 8;
      if(instr.operands[i].memorytype == R)
        pos += 4;
    }
  }
  *position = pos;
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
uint8_t getBitFormAddress(uint8_t *memory, uint16_t address,
                          uint8_t bitNumber) {
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
void setBitInAddress(uint8_t *memory, uint16_t address, uint8_t bitNumber,
                     uint8_t value) {
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
  return (memory[address] << 8) | memory[address + 1];
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
  memory[address + 1] = (uint8_t)value & 0xFF;
}

/**
 * Gets a Double Word from a memory address.
 *
 * @param memory The memory to get the double word from.
 * @param address The address in the memory to get the double word from.
 * @return The value of the double word.
 */
uint32_t getDoubleWordFromAddress(uint8_t *memory, uint16_t address) {
  return ((uint32_t)memory[address] << 24) |
         ((uint32_t)memory[address + 1] << 16) |
         ((uint32_t)memory[address + 2] << 8) | (uint32_t)memory[address + 3];
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
  memory[address + 1] = (uint8_t)(value >> 16) & 0xFF;
  memory[address + 2] = (uint8_t)(value >> 8) & 0xFF;
  memory[address + 3] = (uint8_t)value & 0xFF;
}

/**
 * Gets a Long Word from a memory address.
 *
 * @param memory The memory to get the long word from.
 * @param address The address in the memory to get the long word from.
 * @return The value of the long word.
 */
uint64_t getLongWordFromAddress(uint8_t *memory, uint16_t address) {
  return ((uint64_t)memory[address] << 56) |
         ((uint64_t)memory[address + 1] << 48) |
         ((uint64_t)memory[address + 2] << 40) |
         ((uint64_t)memory[address + 3] << 32) |
         ((uint64_t)memory[address + 4] << 24) |
         ((uint64_t)memory[address + 5] << 16) |
         ((uint64_t)memory[address + 6] << 8) | (uint64_t)memory[address + 7];
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
  memory[address + 1] = (uint8_t)(value >> 48) & 0xFF;
  memory[address + 2] = (uint8_t)(value >> 40) & 0xFF;
  memory[address + 3] = (uint8_t)(value >> 32) & 0xFF;
  memory[address + 4] = (uint8_t)(value >> 24) & 0xFF;
  memory[address + 5] = (uint8_t)(value >> 16) & 0xFF;
  memory[address + 6] = (uint8_t)(value >> 8) & 0xFF;
  memory[address + 7] = (uint8_t)value & 0xFF;
}

/**
 * Gets a int8_t from a operand.
 * 
 * @param oper The operand to get the value from.
 * @param program The program buffer.
 * @param data The data structure containing the memory and register values.
*/
int8_t operandValueToInt8(Operand *oper, uint8_t *program, Data *data)
{
  uint32_t temp;
  float f;
  uint8_t *buffer = NULL;
  if(oper->registertype == I)
  {
    buffer = data->Inputs;
  }
   if(oper->registertype == K)
  {
    buffer = program;
  }
  if(oper->registertype == M)
  {
    buffer = data->Memories;
  }
  if(oper->registertype == Q)
  {
    buffer = data->Outputs;
  }

  switch (oper->memorytype)
  {
  case X:
      return (int8_t)getBitFormAddress(buffer, oper->address, oper->bitNumber);
    break;
  case B:
      return (int8_t)buffer[oper->address];
    break;
  case W:
      return (int8_t)(getWordFromAddress(buffer, oper->address)>>8);
    break;
  case D:
      return (int8_t)(getDoubleWordFromAddress(buffer, oper->address)>>24);
    break;
  case L:
      return (int8_t)(getLongWordFromAddress(buffer, oper->address)>>56);
    break;
  case R:
      temp = getDoubleWordFromAddress(buffer, oper->address);
      f = *(float *)&temp;
      return (int8_t)f;
    break;
  default:
      return 0;
    break;
  }
  return 0;
}

/**
 * Gets a int16_t from a operand.
 * 
 * @param oper The operand to get the value from.
 * @param program The program buffer.
 * @param data The data structure containing the memory and register values.
*/
int16_t operandValueToInt16(Operand *oper, uint8_t *program, Data *data)
{
  uint32_t temp;
  float f;
  uint8_t *buffer = NULL;
  if(oper->registertype == I)
  {
    buffer = data->Inputs;
  }
   if(oper->registertype == K)
  {
    buffer = program;
  }
  if(oper->registertype == M)
  {
    buffer = data->Memories;
  }
  if(oper->registertype == Q)
  {
    buffer = data->Outputs;
  }

  switch (oper->memorytype)
  {
  case X:
      return (int16_t)getBitFormAddress(buffer, oper->address, oper->bitNumber);
    break;
  case B:
      return (int16_t)((int8_t)buffer[oper->address]);
    break;
  case W:
      return (int16_t)getWordFromAddress(buffer, oper->address);
    break;
  case D:
      return (int16_t)(getDoubleWordFromAddress(buffer, oper->address)>>16);
    break;
  case L:
      return (int16_t)(getLongWordFromAddress(buffer, oper->address)>>48);
    break;
  case R:
      temp = getDoubleWordFromAddress(buffer, oper->address);
      f = *(float *)&temp;
      return (int16_t)f;
    break;
  default:
      return 0;
    break;
  }
  return 0;
}

/**
 * Gets a int32_t from a operand.
 * 
 * @param oper The operand to get the value from.
 * @param program The program buffer.
 * @param data The data structure containing the memory and register values.
*/
int32_t operandValueToInt32(Operand *oper, uint8_t *program, Data *data)
{
  uint32_t temp;
  float f;
  uint8_t *buffer = NULL;
  if(oper->registertype == I)
  {
    buffer = data->Inputs;
  }
   if(oper->registertype == K)
  {
    buffer = program;
  }
  if(oper->registertype == M)
  {
    buffer = data->Memories;
  }
  if(oper->registertype == Q)
  {
    buffer = data->Outputs;
  }

  switch (oper->memorytype)
  {
  case X:
      return (int32_t)getBitFormAddress(buffer, oper->address, oper->bitNumber);
    break;
  case B:
      return (int32_t)((int8_t)buffer[oper->address]);
    break;
  case W:
      return (int32_t)(int16_t)getWordFromAddress(buffer, oper->address);
    break;
  case D:
      return (int32_t)getDoubleWordFromAddress(buffer, oper->address);
    break;
  case L:
      return (int32_t)(getLongWordFromAddress(buffer, oper->address)>>32);
    break;
  case R:
      temp = getDoubleWordFromAddress(buffer, oper->address);
      f = *(float *)&temp;
      return (int32_t)f;
    break;
  default:
      return 0;
    break;
  }
  return 0;
}

/**
 * Gets a int64_t from a operand.
 * 
 * @param oper The operand to get the value from.
 * @param program The program buffer.
 * @param data The data structure containing the memory and register values.
*/
int64_t operandValueToInt64(Operand *oper, uint8_t *program, Data *data)
{
  uint32_t temp;
  float f;
  uint8_t *buffer = NULL;
  if(oper->registertype == I)
  {
    buffer = data->Inputs;
  }
   if(oper->registertype == K)
  {
    buffer = program;
  }
  if(oper->registertype == M)
  {
    buffer = data->Memories;
  }
  if(oper->registertype == Q)
  {
    buffer = data->Outputs;
  }

  switch (oper->memorytype)
  {
  case X:
      return (int64_t)getBitFormAddress(buffer, oper->address, oper->bitNumber);
    break;
  case B:
      return (int64_t)((int8_t)buffer[oper->address]);
    break;
  case W:
      return (int64_t)(int16_t)getWordFromAddress(buffer, oper->address);
    break;
  case D:
      return (int64_t)(int32_t)getDoubleWordFromAddress(buffer, oper->address);
    break;
  case L:
      return (int64_t)getLongWordFromAddress(buffer, oper->address);
    break;
  case R:
      temp = getDoubleWordFromAddress(buffer, oper->address);
      f = *(float *)&temp;
      return (int64_t)f;
    break;
  default:
      return 0;
    break;
  }
  return 0;
}

/**
 * Gets a float from a operand.
 * 
 * @param oper The operand to get the value from.
 * @param program The program buffer.
 * @param data The data structure containing the memory and register values.
*/
float operandValueToFloat(Operand *oper, uint8_t *program, Data *data)
{
  uint32_t temp;
  float f;
  uint8_t *buffer = NULL;
  if(oper->registertype == I)
  {
    buffer = data->Inputs;
  }
   if(oper->registertype == K)
  {
    buffer = program;
  }
  if(oper->registertype == M)
  {
    buffer = data->Memories;
  }
  if(oper->registertype == Q)
  {
    buffer = data->Outputs;
  }

  switch (oper->memorytype)
  {
  case X:
      return (float)getBitFormAddress(buffer, oper->address, oper->bitNumber);
    break;
  case B:
      return (float)((int8_t)buffer[oper->address]);
    break;
  case W:
      return (float)(int16_t)getWordFromAddress(buffer, oper->address);
    break;
  case D:
      return (float)getDoubleWordFromAddress(buffer, oper->address);
    break;
  case L:
      return (float)(getLongWordFromAddress(buffer, oper->address)>>32);
    break;
  case R:
      temp = getDoubleWordFromAddress(buffer, oper->address);
      f = *(float *)&temp;
      return f;
    break;
  default:
      return 0.0;
    break;
  }
  return 0.0;
}

/**
 * Executes an instruction.
 *
 * @param instr The instruction to execute.
 * @param data The data structure containing the memory and register values.
 */
void executeInstruction(uint8_t *buffer, Instruction instr, Data *data) {
  int8_t temp8 = 0;
  int16_t temp16 = 0;
  int32_t temp32 = 0;
  int64_t temp64 = 0;
  float tempf = 0;
  switch (instr.opcode) {
  case InstLD:
    if (instr.operands[0].memorytype == X) {
      if (instr.operands[0].registertype == I)
        data->accumulator =
            getBitFormAddress(data->Inputs, instr.operands[0].address,
                              instr.operands[0].bitNumber);
      else if (instr.operands[0].registertype == Q)
        data->accumulator =
            getBitFormAddress(data->Outputs, instr.operands[0].address,
                              instr.operands[0].bitNumber);
      else if (instr.operands[0].registertype == M)
        data->accumulator =
            getBitFormAddress(data->Memories, instr.operands[0].address,
                              instr.operands[0].bitNumber);
      else if (instr.operands[0].registertype == K)
        data->accumulator = 
             operandValueToInt8(&instr.operands[0], buffer, data) == 0 ? 0 : 1;      
    }
    break;
  case InstLDN:
    if (instr.operands[0].memorytype == X) {
      if (instr.operands[0].registertype == I)
        data->accumulator =
            (getBitFormAddress(data->Inputs, instr.operands[0].address,
                               instr.operands[0].bitNumber)) == 0 ? 1 : 0;
      else if (instr.operands[0].registertype == Q)
        data->accumulator =
            (getBitFormAddress(data->Outputs, instr.operands[0].address,
                               instr.operands[0].bitNumber)) == 0 ? 1 : 0;
      else if (instr.operands[0].registertype == M)
        data->accumulator =
            (getBitFormAddress(data->Memories, instr.operands[0].address,
                               instr.operands[0].bitNumber)) == 0 ? 1 : 0;
    else if (instr.operands[0].registertype == K)
        data->accumulator = 
             operandValueToInt8(&instr.operands[0], buffer, data) == 0 ? 1 : 0;      
    }
    break;
  case InstST:
    if (instr.operands[0].memorytype == X) {
      if (instr.operands[0].registertype == Q)
        setBitInAddress(data->Outputs, instr.operands[0].address,
                        instr.operands[0].bitNumber, data->accumulator);
      else if (instr.operands[0].registertype == M)
        setBitInAddress(data->Memories, instr.operands[0].address,
                        instr.operands[0].bitNumber, data->accumulator);
    }
    break;
  case InstSTN:
    if (instr.operands[0].memorytype == X) {
      if (instr.operands[0].registertype == Q)
        setBitInAddress(data->Outputs, instr.operands[0].address,
                        instr.operands[0].bitNumber,
                        (data->accumulator == 0) ? 1 : 0);
      else if (instr.operands[0].registertype == M)
        setBitInAddress(data->Memories, instr.operands[0].address,
                        instr.operands[0].bitNumber,
                        (data->accumulator == 0) ? 1 : 0);
    }
    break;
  case InstS:
    if (data->accumulator == 1) {
      if (instr.operands[0].memorytype == X) {
        if (instr.operands[0].registertype == Q)
          setBitInAddress(data->Outputs, instr.operands[0].address,
                          instr.operands[0].bitNumber, 1);
        else if (instr.operands[0].registertype == M)
          setBitInAddress(data->Memories, instr.operands[0].address,
                          instr.operands[0].bitNumber, 1);
      }
    }
    break;
  case InstR:
    if (data->accumulator == 1) {
      if (instr.operands[0].memorytype == X) {
        if (instr.operands[0].registertype == Q)
          setBitInAddress(data->Outputs, instr.operands[0].address,
                          instr.operands[0].bitNumber, 0);
        else if (instr.operands[0].registertype == M)
          setBitInAddress(data->Memories, instr.operands[0].address,
                          instr.operands[0].bitNumber, 0);
      }
    }
    break;
  case InstMOV:
    if (data->accumulator == 1) {
      if (instr.operands[1].memorytype == X) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        if (instr.operands[1].registertype == Q)
          setBitInAddress(data->Outputs, instr.operands[1].address,
                          instr.operands[1].bitNumber, temp8);
        else if (instr.operands[1].registertype == M)
          setBitInAddress(data->Memories, instr.operands[1].address,
                          instr.operands[1].bitNumber, temp8);
      }
      if (instr.operands[1].memorytype == B) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        if (instr.operands[1].registertype == Q)
          data->Outputs[instr.operands[1].address] = (uint8_t)temp8;
        else if (instr.operands[1].registertype == M)
          data->Memories[instr.operands[1].address] = (uint8_t)temp8;
      }
      if (instr.operands[1].memorytype == W) {
        temp16 = operandValueToInt16(&instr.operands[0], buffer, data);
        if (instr.operands[1].registertype == Q)
          setWordInAddress(data->Outputs, instr.operands[1].address, temp16);
        else if (instr.operands[1].registertype == M)
          setWordInAddress(data->Memories, instr.operands[1].address, temp16);
      }
      if (instr.operands[1].memorytype == D) {
        temp32 = operandValueToInt32(&instr.operands[0], buffer, data);
        if (instr.operands[1].registertype == Q)
          setDoubleWordInAddress(data->Outputs, instr.operands[1].address,
                                 temp32);
        else if (instr.operands[1].registertype == M)
          setDoubleWordInAddress(data->Memories, instr.operands[1].address,
                                 temp32);
      }
      if (instr.operands[1].memorytype == L) {
        temp64 = operandValueToInt64(&instr.operands[0], buffer, data);
        if (instr.operands[1].registertype == Q)
          setLongWordInAddress(data->Outputs, instr.operands[1].address,
                               temp64);
        else if (instr.operands[1].registertype == M)
          setLongWordInAddress(data->Memories, instr.operands[1].address,
                               temp64);
      }
      if (instr.operands[1].memorytype == R) {
        tempf = operandValueToFloat(&instr.operands[0], buffer, data);
        if (instr.operands[1].registertype == Q)
          setDoubleWordInAddress(data->Outputs, instr.operands[1].address,
                                 *(uint32_t *)&tempf);
        else if (instr.operands[1].registertype == M)
          setDoubleWordInAddress(data->Memories, instr.operands[1].address,
                                 *(uint32_t *)&tempf);
      }
    }
    break;
  case InstAND:
    if (instr.operands[0].memorytype == X) {
      if (instr.operands[0].registertype == I)
        data->accumulator =
            data->accumulator & getBitFormAddress(data->Inputs,
                                                  instr.operands[0].address,
                                                  instr.operands[0].bitNumber);
      else if (instr.operands[0].registertype == Q)
        data->accumulator =
            data->accumulator & getBitFormAddress(data->Outputs,
                                                  instr.operands[0].address,
                                                  instr.operands[0].bitNumber);
      else if (instr.operands[0].registertype == M)
        data->accumulator =
            data->accumulator & getBitFormAddress(data->Memories,
                                                  instr.operands[0].address,
                                                  instr.operands[0].bitNumber);
    } else if(instr.operands[0].memorytype == B &&  instr.operands[0].registertype == K){
      data->accumulator =
        data->accumulator & operandValueToInt8(&instr.operands[0], buffer, data);
    }
    break;
  case InstANDp:
    push(stack, InstAND, data->accumulator);
    if (instr.num_operands == 1) {
      if (instr.operands[0].memorytype == X) {
        if (instr.operands[0].registertype == I)
          data->accumulator =
              getBitFormAddress(data->Inputs, instr.operands[0].address,
                                instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == Q)
          data->accumulator =
              getBitFormAddress(data->Outputs, instr.operands[0].address,
                                instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == M)
          data->accumulator =
              getBitFormAddress(data->Memories, instr.operands[0].address,
                                instr.operands[0].bitNumber);
      }
    }
    break;
  case InstANDN:
    if (instr.operands[0].memorytype == X) {
      if (instr.operands[0].registertype == I)
        data->accumulator =
            data->accumulator &
            (getBitFormAddress(data->Inputs, instr.operands[0].address,
                               instr.operands[0].bitNumber) == 0 ? 1 : 0);
      else if (instr.operands[0].registertype == Q)
        data->accumulator =
            data->accumulator &
            (getBitFormAddress(data->Outputs, instr.operands[0].address,
                               instr.operands[0].bitNumber) == 0 ? 1 : 0);
      else if (instr.operands[0].registertype == M)
        data->accumulator =
            data->accumulator &
            (getBitFormAddress(data->Memories, instr.operands[0].address,
                               instr.operands[0].bitNumber) == 0 ? 1 : 0);
    } else if(instr.operands[0].memorytype == B &&  instr.operands[0].registertype == K){
      data->accumulator =
        data->accumulator & (operandValueToInt8(&instr.operands[0], buffer, data) == 0 ? 1 : 0);
    }
    break;
  case InstANDNp:
    push(stack, InstANDN, data->accumulator);
    if (instr.num_operands == 1) {
      if (instr.operands[0].memorytype == X) {
        if (instr.operands[0].registertype == I)
          data->accumulator =
              getBitFormAddress(data->Inputs, instr.operands[0].address,
                                instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == Q)
          data->accumulator =
              getBitFormAddress(data->Outputs, instr.operands[0].address,
                                instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == M)
          data->accumulator =
              getBitFormAddress(data->Memories, instr.operands[0].address,
                                instr.operands[0].bitNumber);
      }
    }
    break;
  case InstOR:
    if (instr.operands[0].memorytype == X) {
      if (instr.operands[0].registertype == I)
        data->accumulator =
            data->accumulator |
            getBitFormAddress(data->Inputs, instr.operands[0].address,
                              instr.operands[0].bitNumber);
      else if (instr.operands[0].registertype == Q)
        data->accumulator =
            data->accumulator |
            getBitFormAddress(data->Outputs, instr.operands[0].address,
                              instr.operands[0].bitNumber);
      else if (instr.operands[0].registertype == M)
        data->accumulator =
            data->accumulator |
            getBitFormAddress(data->Memories, instr.operands[0].address,
                              instr.operands[0].bitNumber);
    } else if(instr.operands[0].memorytype == B &&  instr.operands[0].registertype == K){
      data->accumulator =
        data->accumulator | operandValueToInt8(&instr.operands[0], buffer, data);
    }
    break;
  case InstORp:
    push(stack, InstOR, data->accumulator);
    if (instr.num_operands == 1) {
      if (instr.operands[0].memorytype == X) {
        if (instr.operands[0].registertype == I)
          data->accumulator =
              getBitFormAddress(data->Inputs, instr.operands[0].address,
                                instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == Q)
          data->accumulator =
              getBitFormAddress(data->Outputs, instr.operands[0].address,
                                instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == M)
          data->accumulator =
              getBitFormAddress(data->Memories, instr.operands[0].address,
                                instr.operands[0].bitNumber);
      }
    }
    break;
  case InstORN:
    if (instr.operands[0].memorytype == X) {
      if (instr.operands[0].registertype == I)
        data->accumulator =
            data->accumulator |
            (getBitFormAddress(data->Inputs, instr.operands[0].address,
                               instr.operands[0].bitNumber) == 0
                 ? 1
                 : 0);
      else if (instr.operands[0].registertype == Q)
        data->accumulator =
            data->accumulator |
            (getBitFormAddress(data->Outputs, instr.operands[0].address,
                               instr.operands[0].bitNumber) == 0
                 ? 1
                 : 0);
      else if (instr.operands[0].registertype == M)
        data->accumulator =
            data->accumulator |
            (getBitFormAddress(data->Memories, instr.operands[0].address,
                               instr.operands[0].bitNumber) == 0
                 ? 1
                 : 0);
    } else if(instr.operands[0].memorytype == B &&  instr.operands[0].registertype == K){
      data->accumulator =
        data->accumulator | (operandValueToInt8(&instr.operands[0], buffer, data) == 0 ? 1 : 0);
    }
    break;
  case InstORNp:
    push(stack, InstORN, data->accumulator);
    if (instr.num_operands == 1) {
      if (instr.operands[0].memorytype == X) {
        if (instr.operands[0].registertype == I)
          data->accumulator =
              getBitFormAddress(data->Inputs, instr.operands[0].address,
                                instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == Q)
          data->accumulator =
              getBitFormAddress(data->Outputs, instr.operands[0].address,
                                instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == M)
          data->accumulator =
              getBitFormAddress(data->Memories, instr.operands[0].address,
                                instr.operands[0].bitNumber);
      }
    }
    break;
  case InstXOR:
    if (instr.operands[0].memorytype == X) {
      if (instr.operands[0].registertype == I)
        data->accumulator =
            data->accumulator ^ getBitFormAddress(data->Inputs,
                                                  instr.operands[0].address,
                                                  instr.operands[0].bitNumber);
      else if (instr.operands[0].registertype == Q)
        data->accumulator =
            data->accumulator ^ getBitFormAddress(data->Outputs,
                                                  instr.operands[0].address,
                                                  instr.operands[0].bitNumber);
      else if (instr.operands[0].registertype == M)
        data->accumulator =
            data->accumulator ^ getBitFormAddress(data->Memories,
                                                  instr.operands[0].address,
                                                  instr.operands[0].bitNumber);
    }else if(instr.operands[0].memorytype == B &&  instr.operands[0].registertype == K){
      data->accumulator =
        data->accumulator ^ operandValueToInt8(&instr.operands[0], buffer, data);
    }
    break;
  case InstXORp:
    push(stack, InstXOR, data->accumulator);
    if (instr.num_operands == 1) {
      if (instr.operands[0].memorytype == X) {
        if (instr.operands[0].registertype == I)
          data->accumulator =
              getBitFormAddress(data->Inputs, instr.operands[0].address,
                                instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == Q)
          data->accumulator =
              getBitFormAddress(data->Outputs, instr.operands[0].address,
                                instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == M)
          data->accumulator =
              getBitFormAddress(data->Memories, instr.operands[0].address,
                                instr.operands[0].bitNumber);
      }
    }
    break;
  case InstXORN:
    if (instr.operands[0].memorytype == X) {
      if (instr.operands[0].registertype == I)
        data->accumulator =
            data->accumulator ^
            (getBitFormAddress(data->Inputs, instr.operands[0].address,
                               instr.operands[0].bitNumber) == 0
                 ? 1
                 : 0);
      else if (instr.operands[0].registertype == Q)
        data->accumulator =
            data->accumulator ^
            (getBitFormAddress(data->Outputs, instr.operands[0].address,
                               instr.operands[0].bitNumber) == 0
                 ? 1
                 : 0);
      else if (instr.operands[0].registertype == M)
        data->accumulator =
            data->accumulator ^
            (getBitFormAddress(data->Memories, instr.operands[0].address,
                               instr.operands[0].bitNumber) == 0
                 ? 1
                 : 0);
    }else if(instr.operands[0].memorytype == B &&  instr.operands[0].registertype == K){
      data->accumulator =
        data->accumulator ^ (operandValueToInt8(&instr.operands[0], buffer, data) == 0 ? 1 : 0);
    }
    break;
  case InstXORNp:
    push(stack, InstXORN, data->accumulator);
    if (instr.num_operands == 1) {
      if (instr.operands[0].memorytype == X) {
        if (instr.operands[0].registertype == I)
          data->accumulator =
              getBitFormAddress(data->Inputs, instr.operands[0].address,
                                instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == Q)
          data->accumulator =
              getBitFormAddress(data->Outputs, instr.operands[0].address,
                                instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == M)
          data->accumulator =
              getBitFormAddress(data->Memories, instr.operands[0].address,
                                instr.operands[0].bitNumber);
      }
    }
    break;
  case InstNOT:
    data->accumulator = (data->accumulator == 0) ? 1 : 0;
    break;
  case InstADD:
    if (data->accumulator == 1) {
      if (instr.operands[2].memorytype == X) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        temp8 = temp8 + operandValueToInt8(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setBitInAddress(data->Outputs, instr.operands[2].address,
                          instr.operands[2].bitNumber, temp8);
        else if (instr.operands[2].registertype == M)
          setBitInAddress(data->Memories, instr.operands[2].address,
                          instr.operands[2].bitNumber, temp8);
      }
      if (instr.operands[2].memorytype == B) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        temp8 = temp8 + operandValueToInt8(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          data->Outputs[instr.operands[2].address] = (uint8_t)temp8;
        else if (instr.operands[2].registertype == M)
          data->Memories[instr.operands[2].address] = (uint8_t)temp8;
      }
      if (instr.operands[2].memorytype == W) {
        temp16 = operandValueToInt16(&instr.operands[0], buffer, data);
        temp16 = temp16 + operandValueToInt16(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setWordInAddress(data->Outputs, instr.operands[2].address, temp16);
        else if (instr.operands[2].registertype == M)
          setWordInAddress(data->Memories, instr.operands[2].address, temp16);
      }
      if (instr.operands[2].memorytype == D) {
        temp32 = operandValueToInt32(&instr.operands[0], buffer, data);
        temp32 = temp32 + operandValueToInt32(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setDoubleWordInAddress(data->Outputs, instr.operands[2].address,
                                 temp32);
        else if (instr.operands[2].registertype == M)
          setDoubleWordInAddress(data->Memories, instr.operands[2].address,
                                 temp32);
      }
      if (instr.operands[2].memorytype == L) {
        temp64 = operandValueToInt64(&instr.operands[0], buffer, data);
        temp64 = temp64 + operandValueToInt64(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setLongWordInAddress(data->Outputs, instr.operands[2].address,
                               temp64);
        else if (instr.operands[2].registertype == M)
          setLongWordInAddress(data->Memories, instr.operands[2].address,
                               temp64);
      }
      if (instr.operands[2].memorytype == R) {
        tempf = operandValueToFloat(&instr.operands[0], buffer, data);
        tempf = tempf + operandValueToFloat(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setDoubleWordInAddress(data->Outputs, instr.operands[2].address,
                                 *(uint32_t *)&tempf);
        else if (instr.operands[2].registertype == M)
          setDoubleWordInAddress(data->Memories, instr.operands[2].address,
                                 *(uint32_t *)&tempf);
      }
    }
    break;
  case InstSUB:
    if (data->accumulator == 1) {
      if (instr.operands[2].memorytype == X) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        temp8 = temp8 - operandValueToInt8(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setBitInAddress(data->Outputs, instr.operands[2].address,
                          instr.operands[2].bitNumber, temp8);
        else if (instr.operands[2].registertype == M)
          setBitInAddress(data->Memories, instr.operands[2].address,
                          instr.operands[2].bitNumber, temp8);
      }
      if (instr.operands[2].memorytype == B) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        temp8 = temp8 - operandValueToInt8(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          data->Outputs[instr.operands[2].address] = (uint8_t)temp8;
        else if (instr.operands[2].registertype == M)
          data->Memories[instr.operands[2].address] = (uint8_t)temp8;
      }
      if (instr.operands[2].memorytype == W) {
        temp16 = operandValueToInt16(&instr.operands[0], buffer, data);
        temp16 = temp16 - operandValueToInt16(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setWordInAddress(data->Outputs, instr.operands[2].address, temp16);
        else if (instr.operands[2].registertype == M)
          setWordInAddress(data->Memories, instr.operands[2].address, temp16);
      }
      if (instr.operands[2].memorytype == D) {
        temp32 = operandValueToInt32(&instr.operands[0], buffer, data);
        temp32 = temp32 - operandValueToInt32(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setDoubleWordInAddress(data->Outputs, instr.operands[2].address,
                                 temp32);
        else if (instr.operands[2].registertype == M)
          setDoubleWordInAddress(data->Memories, instr.operands[2].address,
                                 temp32);
      }
      if (instr.operands[2].memorytype == L) {
        temp64 = operandValueToInt64(&instr.operands[0], buffer, data);
        temp64 = temp64 - operandValueToInt64(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setLongWordInAddress(data->Outputs, instr.operands[2].address,
                               temp64);
        else if (instr.operands[2].registertype == M)
          setLongWordInAddress(data->Memories, instr.operands[2].address,
                               temp64);
      }
      if (instr.operands[2].memorytype == R) {
        tempf = operandValueToFloat(&instr.operands[0], buffer, data);
        tempf = tempf - operandValueToFloat(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setDoubleWordInAddress(data->Outputs, instr.operands[2].address,
                                 *(uint32_t *)&tempf);
        else if (instr.operands[2].registertype == M)
          setDoubleWordInAddress(data->Memories, instr.operands[2].address,
                                 *(uint32_t *)&tempf);
      }
    }
    break;
  case InstMUL:
    if (data->accumulator == 1) {
      if (instr.operands[2].memorytype == X) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        temp8 = temp8 * operandValueToInt8(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setBitInAddress(data->Outputs, instr.operands[2].address,
                          instr.operands[2].bitNumber, temp8);
        else if (instr.operands[2].registertype == M)
          setBitInAddress(data->Memories, instr.operands[2].address,
                          instr.operands[2].bitNumber, temp8);
      }
      if (instr.operands[2].memorytype == B) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        temp8 = temp8 * operandValueToInt8(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          data->Outputs[instr.operands[2].address] = (uint8_t)temp8;
        else if (instr.operands[2].registertype == M)
          data->Memories[instr.operands[2].address] = (uint8_t)temp8;
      }
      if (instr.operands[2].memorytype == W) {
        temp16 = operandValueToInt16(&instr.operands[0], buffer, data);
        temp16 = temp16 * operandValueToInt16(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setWordInAddress(data->Outputs, instr.operands[2].address, temp16);
        else if (instr.operands[2].registertype == M)
          setWordInAddress(data->Memories, instr.operands[2].address, temp16);
      }
      if (instr.operands[2].memorytype == D) {
        temp32 = operandValueToInt32(&instr.operands[0], buffer, data);
        temp32 = temp32 * operandValueToInt32(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setDoubleWordInAddress(data->Outputs, instr.operands[2].address,
                                 temp32);
        else if (instr.operands[2].registertype == M)
          setDoubleWordInAddress(data->Memories, instr.operands[2].address,
                                 temp32);
      }
      if (instr.operands[2].memorytype == L) {
        temp64 = operandValueToInt64(&instr.operands[0], buffer, data);
        temp64 = temp64 * operandValueToInt64(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setLongWordInAddress(data->Outputs, instr.operands[2].address,
                               temp64);
        else if (instr.operands[2].registertype == M)
          setLongWordInAddress(data->Memories, instr.operands[2].address,
                               temp64);
      }
      if (instr.operands[2].memorytype == R) {
        tempf = operandValueToFloat(&instr.operands[0], buffer, data);
        tempf = tempf * operandValueToFloat(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setDoubleWordInAddress(data->Outputs, instr.operands[2].address,
                                 *(uint32_t *)&tempf);
        else if (instr.operands[2].registertype == M)
          setDoubleWordInAddress(data->Memories, instr.operands[2].address,
                                 *(uint32_t *)&tempf);
      }
    }
    break;
  case InstDIV:
    if (data->accumulator == 1) {
      if (instr.operands[2].memorytype == X) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        temp8 = temp8 / operandValueToInt8(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setBitInAddress(data->Outputs, instr.operands[2].address,
                          instr.operands[2].bitNumber, temp8);
        else if (instr.operands[2].registertype == M)
          setBitInAddress(data->Memories, instr.operands[2].address,
                          instr.operands[2].bitNumber, temp8);
      }
      if (instr.operands[2].memorytype == B) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        temp8 = temp8 / operandValueToInt8(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          data->Outputs[instr.operands[2].address] = (uint8_t)temp8;
        else if (instr.operands[2].registertype == M)
          data->Memories[instr.operands[2].address] = (uint8_t)temp8;
      }
      if (instr.operands[2].memorytype == W) {
        temp16 = operandValueToInt16(&instr.operands[0], buffer, data);
        temp16 = temp16 / operandValueToInt16(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setWordInAddress(data->Outputs, instr.operands[2].address, temp16);
        else if (instr.operands[2].registertype == M)
          setWordInAddress(data->Memories, instr.operands[2].address, temp16);
      }
      if (instr.operands[2].memorytype == D) {
        temp32 = operandValueToInt32(&instr.operands[0], buffer, data);
        temp32 = temp32 / operandValueToInt32(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setDoubleWordInAddress(data->Outputs, instr.operands[2].address,
                                 temp32);
        else if (instr.operands[2].registertype == M)
          setDoubleWordInAddress(data->Memories, instr.operands[2].address,
                                 temp32);
      }
      if (instr.operands[2].memorytype == L) {
        temp64 = operandValueToInt64(&instr.operands[0], buffer, data);
        temp64 = temp64 / operandValueToInt64(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setLongWordInAddress(data->Outputs, instr.operands[2].address,
                               temp64);
        else if (instr.operands[2].registertype == M)
          setLongWordInAddress(data->Memories, instr.operands[2].address,
                               temp64);
      }
      if (instr.operands[2].memorytype == R) {
        tempf = operandValueToFloat(&instr.operands[0], buffer, data);
        tempf = tempf / operandValueToFloat(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setDoubleWordInAddress(data->Outputs, instr.operands[2].address,
                                 *(uint32_t *)&tempf);
        else if (instr.operands[2].registertype == M)
          setDoubleWordInAddress(data->Memories, instr.operands[2].address,
                                 *(uint32_t *)&tempf);
      }
    }
    break;
  case InstMOD:
    if (data->accumulator == 1) {
      if (instr.operands[2].memorytype == X) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        temp8 = temp8 % operandValueToInt8(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setBitInAddress(data->Outputs, instr.operands[2].address,
                          instr.operands[2].bitNumber, temp8);
        else if (instr.operands[2].registertype == M)
          setBitInAddress(data->Memories, instr.operands[2].address,
                          instr.operands[2].bitNumber, temp8);
      }
      if (instr.operands[2].memorytype == B) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        temp8 = temp8 % operandValueToInt8(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          data->Outputs[instr.operands[2].address] = (uint8_t)temp8;
        else if (instr.operands[2].registertype == M)
          data->Memories[instr.operands[2].address] = (uint8_t)temp8;
      }
      if (instr.operands[2].memorytype == W) {
        temp16 = operandValueToInt16(&instr.operands[0], buffer, data);
        temp16 = temp16 % operandValueToInt16(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setWordInAddress(data->Outputs, instr.operands[2].address, temp16);
        else if (instr.operands[2].registertype == M)
          setWordInAddress(data->Memories, instr.operands[2].address, temp16);
      }
      if (instr.operands[2].memorytype == D) {
        temp32 = operandValueToInt32(&instr.operands[0], buffer, data);
        temp32 = temp32 % operandValueToInt32(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setDoubleWordInAddress(data->Outputs, instr.operands[2].address,
                                 temp32);
        else if (instr.operands[2].registertype == M)
          setDoubleWordInAddress(data->Memories, instr.operands[2].address,
                                 temp32);
      }
      if (instr.operands[2].memorytype == L) {
        temp64 = operandValueToInt64(&instr.operands[0], buffer, data);
        temp64 = temp64 % operandValueToInt64(&instr.operands[1], buffer, data);
        if (instr.operands[2].registertype == Q)
          setLongWordInAddress(data->Outputs, instr.operands[2].address,
                               temp64);
        else if (instr.operands[2].registertype == M)
          setLongWordInAddress(data->Memories, instr.operands[2].address,
                               temp64);
      }
      // No float modulo
    }
    break;
  case InstGT:
    if (data->accumulator == 1) {
      if (instr.operands[1].memorytype == X) {
        temp8 = getBit(operandValueToInt8(&instr.operands[0], buffer, data),instr.operands[0].bitNumber);
        data->accumulator = (temp8 > getBit(operandValueToInt8(&instr.operands[1], buffer, data),
                            instr.operands[1].bitNumber) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == B) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        data->accumulator = (temp8 > operandValueToInt8(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == W) {
        temp16 = operandValueToInt16(&instr.operands[0], buffer, data);
        data->accumulator = (temp16 > operandValueToInt16(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == D) {
        temp32 = operandValueToInt32(&instr.operands[0], buffer, data);
        data->accumulator = (temp32 > operandValueToInt32(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == L) {
        temp64 = operandValueToInt64(&instr.operands[0], buffer, data);
        data->accumulator = (temp64 > operandValueToInt64(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == R) {
        tempf = operandValueToFloat(&instr.operands[0], buffer, data);
        data->accumulator = (tempf > operandValueToFloat(&instr.operands[1], buffer, data) ? 1 : 0);
      }
    }
    break;
  case InstGE:
    if (data->accumulator == 1) {
      if (instr.operands[1].memorytype == X) {
        temp8 = getBit(operandValueToInt8(&instr.operands[0], buffer, data),instr.operands[0].bitNumber);
        data->accumulator = (temp8 >= getBit(operandValueToInt8(&instr.operands[1], buffer, data),instr.operands[1].bitNumber) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == B) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        data->accumulator = (temp8 >= operandValueToInt8(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == W) {
        temp16 = operandValueToInt16(&instr.operands[0], buffer, data);
        data->accumulator = (temp16 >= operandValueToInt16(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == D) {
        temp32 = operandValueToInt32(&instr.operands[0], buffer, data);
        data->accumulator = (temp32 >= operandValueToInt32(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == L) {
        temp64 = operandValueToInt64(&instr.operands[0], buffer, data);
        data->accumulator = (temp64 >= operandValueToInt64(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == R) {
        tempf = operandValueToFloat(&instr.operands[0], buffer, data);
        data->accumulator = (tempf >= operandValueToFloat(&instr.operands[1], buffer, data) ? 1 : 0);
      }
    }
    break;
  case InstEQ:
    if (data->accumulator == 1) {
      if (instr.operands[1].memorytype == X) {
        temp8 = getBit(operandValueToInt8(&instr.operands[0], buffer, data),instr.operands[0].bitNumber);
        data->accumulator = (temp8 == getBit(operandValueToInt8(&instr.operands[1], buffer, data),instr.operands[1].bitNumber) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == B) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        data->accumulator = (temp8 == operandValueToInt8(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == W) {
        temp16 = operandValueToInt16(&instr.operands[0], buffer, data);
        data->accumulator = (temp16 == operandValueToInt16(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == D) {
        temp32 = operandValueToInt32(&instr.operands[0], buffer, data);
        data->accumulator = (temp32 == operandValueToInt32(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == L) {
        temp64 = operandValueToInt64(&instr.operands[0], buffer, data);
        data->accumulator = (temp64 == operandValueToInt64(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == R) {
        tempf = operandValueToFloat(&instr.operands[0], buffer, data);
        data->accumulator = (tempf == operandValueToFloat(&instr.operands[1], buffer, data) ? 1 : 0);
      }
    }
    break;
  case InstNE:
    if (data->accumulator == 1) {
      if (instr.operands[1].memorytype == X) {
        temp8 = getBit(operandValueToInt8(&instr.operands[0], buffer, data),instr.operands[0].bitNumber);
        data->accumulator = (temp8 != getBit(operandValueToInt8(&instr.operands[1], buffer, data),instr.operands[1].bitNumber) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == B) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        data->accumulator = (temp8 != operandValueToInt8(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == W) {
        temp16 = operandValueToInt16(&instr.operands[0], buffer, data);
        data->accumulator = (temp16 != operandValueToInt16(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == D) {
        temp32 = operandValueToInt32(&instr.operands[0], buffer, data);
        data->accumulator = (temp32 != operandValueToInt32(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == L) {
        temp64 = operandValueToInt64(&instr.operands[0], buffer, data);
        data->accumulator = (temp64 != operandValueToInt64(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == R) {
        tempf = operandValueToFloat(&instr.operands[0], buffer, data);
        data->accumulator = (tempf != operandValueToFloat(&instr.operands[1], buffer, data) ? 1 : 0);
      }
    }
    break;
  case InstLT:
    if (data->accumulator == 1) {
      if (instr.operands[1].memorytype == X) {
        temp8 = getBit(operandValueToInt8(&instr.operands[0], buffer, data),instr.operands[0].bitNumber);
        data->accumulator = (temp8 < getBit(operandValueToInt8(&instr.operands[1], buffer, data),instr.operands[1].bitNumber) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == B) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        data->accumulator = (temp8 < operandValueToInt8(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == W) {
        temp16 = operandValueToInt16(&instr.operands[0], buffer, data);
        data->accumulator = (temp16 < operandValueToInt16(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == D) {
        temp32 = operandValueToInt32(&instr.operands[0], buffer, data);
        data->accumulator = (temp32 < operandValueToInt32(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == L) {
        temp64 = operandValueToInt64(&instr.operands[0], buffer, data);
        data->accumulator = (temp64 < operandValueToInt64(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == R) {
        tempf = operandValueToFloat(&instr.operands[0], buffer, data);
        data->accumulator = (tempf < operandValueToFloat(&instr.operands[1], buffer, data) ? 1 : 0);
      }
    }
    break;
  case InstLE:
      if (data->accumulator == 1) {
      if (instr.operands[1].memorytype == X) {
        temp8 = getBit(operandValueToInt8(&instr.operands[0], buffer, data),instr.operands[0].bitNumber);
        data->accumulator = (temp8 <= getBit(operandValueToInt8(&instr.operands[1], buffer, data),instr.operands[1].bitNumber) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == B) {
        temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
        data->accumulator = (temp8 <= operandValueToInt8(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == W) {
        temp16 = operandValueToInt16(&instr.operands[0], buffer, data);
        data->accumulator = (temp16 <= operandValueToInt16(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == D) {
        temp32 = operandValueToInt32(&instr.operands[0], buffer, data);
        data->accumulator = (temp32 <= operandValueToInt32(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == L) {
        temp64 = operandValueToInt64(&instr.operands[0], buffer, data);
        data->accumulator = (temp64 <= operandValueToInt64(&instr.operands[1], buffer, data) ? 1 : 0);
      }
      if (instr.operands[1].memorytype == R) {
        tempf = operandValueToFloat(&instr.operands[0], buffer, data);
        data->accumulator = (tempf <= operandValueToFloat(&instr.operands[1], buffer, data) ? 1 : 0);
      }
    }
    break;
  case Instq:
    pop(stack, &poppedElement);
    if(poppedElement.instruction == InstAND ||poppedElement.instruction == InstOR ||
      poppedElement.instruction == InstANDN ||poppedElement.instruction == InstORN ||
      poppedElement.instruction == InstXOR ||poppedElement.instruction == InstXORN ){
        uint8_t lbuffer[2];
        uint8_t nop;
        nop=getNumOp(poppedElement.instruction); 
        Operand ops[MaxOpers];
        ops[0]={B,K,0,0};
        Instruction in;
        in = {poppedElement.instruction,nop,{*ops}};
        lbuffer[0] = poppedElement.value;
        lbuffer[1] = 0;
        executeInstruction(lbuffer,in, data);
    }
    /*
    switch (poppedElement.instruction) {
      case InstANDp:
        data->accumulator = data->accumulator & poppedElement.value;
        break;
      case InstANDNp:
        data->accumulator =
            (data->accumulator & poppedElement.value) == 0 ? 1 : 0;
        break;
      case InstORp:
        data->accumulator = data->accumulator | poppedElement.value;
        break;
      case InstORNp:
        data->accumulator =
            (data->accumulator | poppedElement.value) == 0 ? 1 : 0;
        break;
      case InstXORp:
        data->accumulator = data->accumulator ^ poppedElement.value;
        break;
      case InstXORNp:
        data->accumulator =
            (data->accumulator ^ poppedElement.value) == 0 ? 1 : 0;
        break;
      default:
        break;
    }*/
    break;
  case InstTON: // TON(ntimer, IN, ticks, prescaler, OUT) Example TON(K5,
                // IX0.0, K10,K1,QX0.1

    temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
    if (instr.operands[1].registertype == I) {
      timers[temp8].IN =
          (getBitFormAddress(data->Inputs, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    } else if (instr.operands[1].registertype == M) {
      timers[temp8].IN =
          (getBitFormAddress(data->Memories, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    } else if (instr.operands[1].registertype == Q) {
      timers[temp8].IN =
          (getBitFormAddress(data->Outputs, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    }

    if (instr.operands[2].registertype == K) {
      timers[temp8].PT = operandValueToInt16(&instr.operands[2], buffer, data);
    } else if (instr.operands[2].registertype == M) {
      timers[temp8].PT =
          getWordFromAddress(data->Memories, instr.operands[2].address);
    }

    if (instr.operands[3].registertype == K) {
      timers[temp8].prescaler =
          operandValueToInt8(&instr.operands[3], buffer, data);
    } else if (instr.operands[3].registertype == M) {
      timers[temp8].prescaler = (uint8_t)(getWordFromAddress(
          data->Memories, instr.operands[2].address));
    }

    runTimerTON(&timers[temp8]);

    if (instr.operands[4].registertype == Q) {
      setBitInAddress(data->Outputs, instr.operands[4].address,
                      instr.operands[4].bitNumber, timers[temp8].QO);
    } else if (instr.operands[4].registertype == M) {
      setBitInAddress(data->Memories, instr.operands[4].address,
                      instr.operands[4].bitNumber, timers[temp8].QO);
    }
    break;

  case InstTOF: // TOF(ntimer, IN, ticks, prescaler, OUT) Example TOF(K5,
                // IX0.0, K10,K1,QX0.1
    temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
    if (instr.operands[1].registertype == I) {
      timers[temp8].IN =
          (getBitFormAddress(data->Inputs, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    } else if (instr.operands[1].registertype == M) {
      timers[temp8].IN =
          (getBitFormAddress(data->Memories, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    } else if (instr.operands[1].registertype == Q) {
      timers[temp8].IN =
          (getBitFormAddress(data->Outputs, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    }

    if (instr.operands[2].registertype == K) {
      timers[temp8].PT = operandValueToInt16(&instr.operands[2], buffer, data);
    } else if (instr.operands[2].registertype == M) {
      timers[temp8].PT =
          getWordFromAddress(data->Memories, instr.operands[2].address);
    }

    if (instr.operands[3].registertype == K) {
      timers[temp8].prescaler =
          operandValueToInt8(&instr.operands[3], buffer, data);
    } else if (instr.operands[3].registertype == M) {
      timers[temp8].prescaler = (uint8_t)(getWordFromAddress(
          data->Memories, instr.operands[2].address));
    }

    runTimerTOF(&timers[temp8]);

    if (instr.operands[4].registertype == Q) {
      setBitInAddress(data->Outputs, instr.operands[4].address,
                      instr.operands[4].bitNumber, timers[temp8].QO);
    } else if (instr.operands[4].registertype == M) {
      setBitInAddress(data->Memories, instr.operands[4].address,
                      instr.operands[4].bitNumber, timers[temp8].QO);
    }
    break;

  case InstTP: // TOF(ntimer, IN, PT, prescaler, OUT) Example TOF(K5,
               // IX0.0, K10,K1,QX0.1
    temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
    if (instr.operands[1].registertype == I) {
      timers[temp8].IN =
          (getBitFormAddress(data->Inputs, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    } else if (instr.operands[1].registertype == M) {
      timers[temp8].IN =
          (getBitFormAddress(data->Memories, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    } else if (instr.operands[1].registertype == Q) {
      timers[temp8].IN =
          (getBitFormAddress(data->Outputs, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    }

    if (instr.operands[2].registertype == K) {
      timers[temp8].PT = operandValueToInt16(&instr.operands[2], buffer, data);
    } else if (instr.operands[2].registertype == M) {
      timers[temp8].PT =
          getWordFromAddress(data->Memories, instr.operands[2].address);
    }

    if (instr.operands[3].registertype == K) {
      timers[temp8].prescaler =
          operandValueToInt8(&instr.operands[3], buffer, data);
    } else if (instr.operands[3].registertype == M) {
      timers[temp8].prescaler = (uint8_t)(getWordFromAddress(
          data->Memories, instr.operands[2].address));
    }

    runTimerTP(&timers[temp8]);

    if (instr.operands[4].registertype == Q) {
      setBitInAddress(data->Outputs, instr.operands[4].address,
                      instr.operands[4].bitNumber, timers[temp8].QO);
    } else if (instr.operands[4].registertype == M) {
      setBitInAddress(data->Memories, instr.operands[4].address,
                      instr.operands[4].bitNumber, timers[temp8].QO);
    }
    break;

    case InstCTU: // CTU(ncouter, CO, PV, OUT); example CTU(K3, IX0.3, k4,QX0.6)
                  // C0-> Input for counting
                  //PV -> Set point for counting
      temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
      if (instr.operands[1].registertype == I) {
        counters[temp8].CO =
            (getBitFormAddress(data->Inputs, instr.operands[1].address,
                               instr.operands[1].bitNumber) == 0
                 ? 0
                 : 1);
      } else if (instr.operands[1].registertype == M) {
        counters[temp8].CO =
            (getBitFormAddress(data->Memories, instr.operands[1].address,
                               instr.operands[1].bitNumber) == 0
                 ? 0
                 : 1);
      } else if (instr.operands[1].registertype == Q) {
        counters[temp8].CO =
            (getBitFormAddress(data->Outputs, instr.operands[1].address,
                               instr.operands[1].bitNumber) == 0
                 ? 0
                 : 1);
      }

      if (instr.operands[2].registertype == K) {
        counters[temp8].PV = operandValueToInt16(&instr.operands[2], buffer, data);
      } else if (instr.operands[2].registertype == M) {
        counters[temp8].PV =
            getWordFromAddress(data->Memories, instr.operands[2].address);
      }

      if (instr.operands[3].registertype == I) {
        counters[temp8].R_LD =
            (getBitFormAddress(data->Inputs, instr.operands[3].address,
                               instr.operands[3].bitNumber) == 0
                 ? 0
                 : 1);
      } else if (instr.operands[3].registertype == M) {
        counters[temp8].R_LD =
            (getBitFormAddress(data->Memories, instr.operands[3].address,
                               instr.operands[3].bitNumber) == 0
                 ? 0
                 : 1);
      } else if (instr.operands[3].registertype == Q) {
        counters[temp8].R_LD =
            (getBitFormAddress(data->Outputs, instr.operands[3].address,
                               instr.operands[3].bitNumber) == 0
                 ? 0
                 : 1);
      }


      runCounterUp(&counters[temp8]);

      if (instr.operands[4].registertype == Q) {
        setBitInAddress(data->Outputs, instr.operands[4].address,
                        instr.operands[4].bitNumber, counters[temp8].QO);
      } else if (instr.operands[4].registertype == M) {
        setBitInAddress(data->Memories, instr.operands[4].address,
                        instr.operands[4].bitNumber, counters[temp8].QO);
      }
    break;


  case InstCTD: // CTU(ncouter, CO, PV, OUT); example CTU(K3, IX0.3, k4,QX0.6)
                // C0-> Input for counting
                //PV -> Set point for counting
    temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
    if (instr.operands[1].registertype == I) {
      counters[temp8].CO =
          (getBitFormAddress(data->Inputs, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    } else if (instr.operands[1].registertype == M) {
      counters[temp8].CO =
          (getBitFormAddress(data->Memories, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    } else if (instr.operands[1].registertype == Q) {
      counters[temp8].CO =
          (getBitFormAddress(data->Outputs, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    }

    if (instr.operands[2].registertype == K) {
      counters[temp8].PV = operandValueToInt16(&instr.operands[2], buffer, data);
    } else if (instr.operands[2].registertype == M) {
      counters[temp8].PV =
          getWordFromAddress(data->Memories, instr.operands[2].address);
    }

    if (instr.operands[3].registertype == I) {
      counters[temp8].R_LD =
          (getBitFormAddress(data->Inputs, instr.operands[3].address,
                             instr.operands[3].bitNumber) == 0
               ? 0
               : 1);
    } else if (instr.operands[3].registertype == M) {
      counters[temp8].R_LD =
          (getBitFormAddress(data->Memories, instr.operands[3].address,
                             instr.operands[3].bitNumber) == 0
               ? 0
               : 1);
    } else if (instr.operands[3].registertype == Q) {
      counters[temp8].R_LD =
          (getBitFormAddress(data->Outputs, instr.operands[3].address,
                             instr.operands[3].bitNumber) == 0
               ? 0
               : 1);
    }


    runCounterDown(&counters[temp8]);

    if (instr.operands[4].registertype == Q) {
      setBitInAddress(data->Outputs, instr.operands[4].address,
                      instr.operands[4].bitNumber, counters[temp8].QO);
    } else if (instr.operands[4].registertype == M) {
      setBitInAddress(data->Memories, instr.operands[4].address,
                      instr.operands[4].bitNumber, counters[temp8].QO);
    }
    break;
  case InstRTRIGGER://R_TRIGGER (ntrigger,IN, QO)
    temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
    if (instr.operands[1].registertype == I) {
      triggers[temp8].CLK =
          (getBitFormAddress(data->Inputs, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    } else if (instr.operands[1].registertype == M) {
      triggers[temp8].CLK =
          (getBitFormAddress(data->Memories, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    } else if (instr.operands[1].registertype == Q) {
      triggers[temp8].CLK =
          (getBitFormAddress(data->Outputs, instr.operands[1].address,
                             instr.operands[1].bitNumber) == 0
               ? 0
               : 1);
    }
    runRTrigger(&triggers[temp8]);

    if (instr.operands[2].registertype == Q) {
      setBitInAddress(data->Outputs, instr.operands[2].address,
                      instr.operands[2].bitNumber, triggers[temp8].QO);
    } else if (instr.operands[2].registertype == M) {
      setBitInAddress(data->Memories, instr.operands[2].address,
                      instr.operands[2].bitNumber, triggers[temp8].QO);
    }
    break;

    case InstFTRIGGER://R_TRIGGER (ntrigger,IN, QO)
      temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
      if (instr.operands[1].registertype == I) {
        triggers[temp8].CLK =
            (getBitFormAddress(data->Inputs, instr.operands[1].address,
                               instr.operands[1].bitNumber) == 0
                 ? 0
                 : 1);
      } else if (instr.operands[1].registertype == M) {
        triggers[temp8].CLK =
            (getBitFormAddress(data->Memories, instr.operands[1].address,
                               instr.operands[1].bitNumber) == 0
                 ? 0
                 : 1);
      } else if (instr.operands[1].registertype == Q) {
        triggers[temp8].CLK =
            (getBitFormAddress(data->Outputs, instr.operands[1].address,
                               instr.operands[1].bitNumber) == 0
                 ? 0
                 : 1);
      }
      runFTrigger(&triggers[temp8]);
      if (instr.operands[2].registertype == Q) {
        setBitInAddress(data->Outputs, instr.operands[2].address,
                        instr.operands[2].bitNumber, triggers[temp8].QO);
      } else if (instr.operands[2].registertype == M) {
        setBitInAddress(data->Memories, instr.operands[2].address,
                        instr.operands[2].bitNumber, triggers[temp8].QO);
      }
      break;
    // TODO:  CTU, CTD, TON, TOF etc
  default:
    break;
  }
}

/**
 * Initializes the memory.
 *
 * @param data The data structure containing the memory and register values.
 */
void initializeMemory(Data *data, Timer *atimers, Counter *acounters, Trigger *atriggers, Stack *astack) {
  for (uint16_t i = 0; i < MemorySize; i++) {
    data->Memories[i] = 0;
  }
  for (uint16_t i = 0; i < InputSize; i++) {
    data->Inputs[i] = 0;
  }
  for (uint16_t i = 0; i < OutputSize; i++) {
    data->Outputs[i] = 0;
  }

  timers = atimers;
  counters = acounters;
  triggers = atriggers;
  stack = astack;
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

uint8_t verifyProgramIntegrity(uint8_t *buffer) {
  uint32_t calculatedSize = 0;
  uint32_t spectedSize;
  uint16_t programSize = getProgramSize(buffer);
  for (uint16_t i = 0; i < programSize; i++) {
    calculatedSize += buffer[i];
  }
  spectedSize = (buffer[programSize] << 24) | (buffer[programSize + 1] << 16) |
                (buffer[programSize + 2] << 8) | buffer[programSize + 3];
  if(calculatedSize == spectedSize)
    return noError;
  else
    return criticalError;
}