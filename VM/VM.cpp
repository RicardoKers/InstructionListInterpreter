/* Virtual Machine for a PLC (Programmable Logic Controller) using a simple
instruction set.

The information storage uses the following nomenclature:
I Boolean inputs;
Q Boolean outputs;
M memories;
K constants;
==============================
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

The last 4 bytes of the program are the Checksum of the program.
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
31 CTU (Counter Up): CTU(ncouter, CO, PV, RST, OUT, CV);
32 CTD (Counter Down): CTD(ncouter, CO, PV, LD, OUT, CV);
33 TON (Timer On Delay): TON(ntimer, IN, PT, prescaler, OUT); Example TON(K5, IX0.0, 10,1,QX0.1)
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
  uint8_t n[] = {
    NumOpLD, NumOpLDN, NumOpST, NumOpSTN, NumOpS,
    NumOpR, NumOpMOV, NumOpAND, NumOpANDp, NumOpANDN,
    NumOpANDNp, NumOpOR, NumOpORp, NumOpORN, NumOpORNp,
    NumOpXOR, NumOpXORp, NumOpXORN, NumOpXORNp, NumOpNOT,
    NumOpADD, NumOpSUB, NumOpMUL, NumOpDIV, NumOpMOD,
    NumOpGT, NumOpGE, NumOpEQ, NumOpNE, NumOpLT,
    NumOpLE, NumOpCTU, NumOpCTD, NumOpTON, NumOpTOF,
    NumOpTP, NumOpRTRIGGER, NumOpFTRIGGER, NumOpq
  };
  return(n[inst]);
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
  return getBit(memory[address], bitNumber);
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
  DataUnion u;
  u.u8 = (uint8_t *)(memory + address);
  return (u.i16[0]);
}

/**
 * Sets a Word in a memory address.
 *
 * @param memory The memory to set the byte in.
 * @param address The address in the memory to set the byte in.
 * @param value The value to set the byte to.
 */
void setWordInAddress(uint8_t *memory, uint16_t address, int16_t value) {
  DataUnion u;
  u.u8 = (uint8_t *)(memory + address);
  u.i16[0] = value;
}

/**
 * Gets a Double Word from a memory address.
 *
 * @param memory The memory to get the double word from.
 * @param address The address in the memory to get the double word from.
 * @return The value of the double word.
 */
int32_t getDoubleWordFromAddress(uint8_t *memory, uint16_t address) {
  DataUnion u;
  u.u8 = (uint8_t *)(memory + address);
  return (u.i32[0]);
}

/**
 * Sets a Double Word in a memory address.
 *
 * @param memory The memory to set the double word in.
 * @param address The address in the memory to set the double word in.
 * @param value The value to set the double word to.
 */
void setDoubleWordInAddress(uint8_t *memory, uint16_t address, uint32_t value) {
  DataUnion u;
  u.u8 = (uint8_t *)(memory + address);
  u.i32[0] = value;
}

/**
 * Gets a Long Word from a memory address.
 *
 * @param memory The memory to get the long word from.
 * @param address The address in the memory to get the long word from.
 * @return The value of the long word.
 */
int64_t getLongWordFromAddress(uint8_t *memory, uint16_t address) {
  DataUnion u;
  u.u8 = (uint8_t *)(memory + address);
  return (u.i64[0]);
}

/**
 * Sets a Long Word in a memory address.
 *
 * @param memory The memory to set the long word in.
 * @param address The address in the memory to set the long word in.
 * @param value The value to set the long word to.
 */
void setLongWordInAddress(uint8_t *memory, uint16_t address, uint64_t value) {
  DataUnion u;
  u.u8 = (uint8_t *)(memory + address);
  u.i64[0] = value;
}

/**
 * Gets a float from a memory address.
 *
 * @param memory The memory to get the float from.
 * @param address The address in the memory to get the float from.
 * @return The value of the float.
 */
float getFloatFromAddress(uint8_t *memory, uint16_t address) {
  DataUnion u;
  u.u8 = (uint8_t *)(memory + address);
  return (u.f[0]);
}

/**
 * Sets a float in a memory address.
 *
 * @param memory The memory to set the float in.
 * @param address The address in the memory to set the float in.
 * @param value The value to set the float to.
 */
void setFloatInAddress(uint8_t *memory, uint16_t address, float value) {
  DataUnion u;
  u.u8 = (uint8_t *)(memory + address);
  u.f[0] = value;
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
  if(oper->memorytype == R){
    return (int8_t)getFloatFromAddress(buffer, oper->address);
  }
  return (int8_t)buffer[oper->address];
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
    if(oper->memorytype == R){
    return (int16_t)getFloatFromAddress(buffer, oper->address);
  }
  return getWordFromAddress(buffer, oper->address);
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
    if(oper->memorytype == R){
    return (int32_t)getFloatFromAddress(buffer, oper->address);
  }
  return getDoubleWordFromAddress(buffer, oper->address);
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
    if(oper->memorytype == R){
    return (int64_t)getFloatFromAddress(buffer, oper->address);
  }
  return getLongWordFromAddress(buffer, oper->address);
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
  return getFloatFromAddress(buffer, oper->address);
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
      instr.operands[i].address = getWordFromAddress(buffer, pos);
      pos += 2;
    }
    else
    {
      instr.operands[i].address = pos;
      if(instr.operands[i].memorytype == X)
        pos += 1;
      else if(instr.operands[i].memorytype == B)
        pos += 1;
      else if(instr.operands[i].memorytype == W)
        pos += 2;
      else if(instr.operands[i].memorytype == D)
        pos += 4;
      else if(instr.operands[i].memorytype == L)
        pos += 8;
      else if(instr.operands[i].memorytype == R)
        pos += 4;
    }
  }
  *position = pos;
  return instr;
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
          setFloatInAddress(data->Outputs, instr.operands[1].address, tempf);
        else if (instr.operands[1].registertype == M)
          setFloatInAddress(data->Memories, instr.operands[1].address,tempf);
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
          setFloatInAddress(data->Outputs, instr.operands[2].address, tempf);
        else if (instr.operands[2].registertype == M)
          setFloatInAddress(data->Memories, instr.operands[2].address, tempf);
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
          setFloatInAddress(data->Outputs, instr.operands[2].address, tempf);
        else if (instr.operands[2].registertype == M)
          setFloatInAddress(data->Memories, instr.operands[2].address, tempf);
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
          setFloatInAddress(data->Outputs, instr.operands[2].address, tempf);
        else if (instr.operands[2].registertype == M)
          setFloatInAddress(data->Memories, instr.operands[2].address, tempf);
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
          setFloatInAddress(data->Outputs, instr.operands[2].address, tempf);
        else if (instr.operands[2].registertype == M)
          setFloatInAddress(data->Memories, instr.operands[2].address, tempf);
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
        //ops[0]={B,K,0,0};
		    ops[0].memorytype=B;
		    ops[0].registertype=K;
		    ops[0].bitNumber=0;
		    ops[0].address=0;
		    Instruction in;
		    //in = {poppedElement.instruction,nop,{*ops}};
		    in.opcode = poppedElement.instruction;
		    in.num_operands = nop;
		    for(int i=0;i<nop;i++){
			    in.operands[i]=ops[i];
		    }
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

    setWordInAddress(data->Memories, instr.operands[5].address, timers[temp8].ET);

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

    setWordInAddress(data->Memories, instr.operands[5].address, timers[temp8].ET);

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

    setWordInAddress(data->Memories, instr.operands[5].address, timers[temp8].ET);

    break;

    case InstCTU: // CTU(ncouter, CO, PV, RST, OUT, CV); example CTU(K3, IX0.3, k4, IX0.1, QX0.6, MW0)
                  // C0-> Input for counting
                  // PV -> Set point for counting
                  // RST -> Reset counter
                  // OUT -> Output
                  // CV -> Current value of the counter
      temp8 = operandValueToInt8(&instr.operands[0], buffer, data);
      if (instr.operands[1].registertype == I) {
        counters[temp8].CO =
            (getBitFormAddress(data->Inputs, instr.operands[1].address,
                               instr.operands[1].bitNumber) == 0 ? 0 : 1);
      } else if (instr.operands[1].registertype == M) {
        counters[temp8].CO =
            (getBitFormAddress(data->Memories, instr.operands[1].address,
                               instr.operands[1].bitNumber) == 0 ? 0 : 1);
      } else if (instr.operands[1].registertype == Q) {
        counters[temp8].CO =
            (getBitFormAddress(data->Outputs, instr.operands[1].address,
                               instr.operands[1].bitNumber) == 0 ? 0 : 1);
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

      setWordInAddress(data->Memories, instr.operands[5].address, counters[temp8].CV);

    break;


  case InstCTD: // CTU(ncouter, CO, PV, OUT, LD, CV); example CTU(K3, IX0.3, k4, IX0.1, QX0.6, MW0)
                // C0-> Input for counting
                // PV -> Set point for counting
                // LD -> Load counter
                // OUT -> Output
                // CV -> Current value of the counter
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

      setWordInAddress(data->Memories, instr.operands[5].address, counters[temp8].CV);

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
  DataUnion u;
  u.u8 = buffer;
  return (u.u16[0]);
}

/**
 * Verifies the integrity of the program.
 * 
 * @param buffer The buffer containing the program.
 * @return The error code.
 */
uint8_t verifyProgramIntegrity(uint8_t *buffer) {
  uint32_t calculatedSize = 0;
  uint32_t spectedSize;
  uint16_t programSize = getProgramSize(buffer);
  for (uint16_t i = 0; i < programSize; i++) {
    calculatedSize += buffer[i];
  }
  spectedSize = getDoubleWordFromAddress(buffer,programSize);
  if(calculatedSize == spectedSize)
    return noError;
  else
    return criticalError;
}