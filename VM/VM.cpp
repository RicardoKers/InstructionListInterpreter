#include "VM.h"
#include "stack.h"
#include "timer.h"

StackElementb poppedElement;
Stackb stackb;

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
  instr.opcode = (buffer[pos] >> 2);
  instr.num_operands = buffer[pos] & 0x03;
  for (uint16_t i = 0; i < instr.num_operands; i++) {
    instr.operands[i].memorytype = buffer[pos + 1 + (i * 3)] >> 5;
    instr.operands[i].registertype = (buffer[pos + 1 + (i * 3)] >> 3) & 0x03;
    instr.operands[i].bitNumber = buffer[pos + 1 + (i * 3)] & 0x07;
    instr.operands[i].address =
        (buffer[pos + 2 + (i * 3)] << 8) | buffer[pos + 3 + (i * 3)];
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
 * Executes an instruction.
 *
 * @param instr The instruction to execute.
 * @param data The data structure containing the memory and register values.
 */
void executeInstruction(Instruction instr, Data *data) {
  uint8_t temp = 0;
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
    }
    break;
  case InstLDN:
    if (instr.operands[0].memorytype == X) {
      if (instr.operands[0].registertype == I)
        data->accumulator =
            (getBitFormAddress(data->Inputs, instr.operands[0].address,
                               instr.operands[0].bitNumber)) == 0
                ? 1
                : 0;
      else if (instr.operands[0].registertype == Q)
        data->accumulator =
            (getBitFormAddress(data->Outputs, instr.operands[0].address,
                               instr.operands[0].bitNumber)) == 0
                ? 1
                : 0;
      else if (instr.operands[0].registertype == M)
        data->accumulator =
            (getBitFormAddress(data->Memories, instr.operands[0].address,
                               instr.operands[0].bitNumber)) == 0
                ? 1
                : 0;
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
      if (instr.operands[0].memorytype == X) {
        if (instr.operands[0].registertype == I)
          temp = getBitFormAddress(data->Inputs, instr.operands[0].address,
                                   instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == Q)
          temp = getBitFormAddress(data->Outputs, instr.operands[0].address,
                                   instr.operands[0].bitNumber);
        else if (instr.operands[0].registertype == M)
          temp = getBitFormAddress(data->Memories, instr.operands[0].address,
                                   instr.operands[0].bitNumber);
      }
      if (instr.operands[1].memorytype == X) {
        if (instr.operands[1].registertype == Q)
          setBitInAddress(data->Outputs, instr.operands[1].address,
                          instr.operands[1].bitNumber, temp);
        else if (instr.operands[1].registertype == M)
          setBitInAddress(data->Memories, instr.operands[1].address,
                          instr.operands[1].bitNumber, temp);
      }
      // TODO: add move for other types of memory
    }
    break;
  case InstMOVp:
    // TODO: add stack support
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
    }
    break;
  case InstANDp:
    pushb(&stackb, InstANDp, data->accumulator);
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
                               instr.operands[0].bitNumber) == 0
                 ? 1
                 : 0);
      else if (instr.operands[0].registertype == Q)
        data->accumulator =
            data->accumulator &
            (getBitFormAddress(data->Outputs, instr.operands[0].address,
                               instr.operands[0].bitNumber) == 0
                 ? 1
                 : 0);
      else if (instr.operands[0].registertype == M)
        data->accumulator =
            data->accumulator &
            (getBitFormAddress(data->Memories, instr.operands[0].address,
                               instr.operands[0].bitNumber) == 0
                 ? 1
                 : 0);
    }
    break;
  case InstANDNp:
    pushb(&stackb, InstANDNp, data->accumulator);
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
    }
    break;
  case InstORp:
    pushb(&stackb, InstORp, data->accumulator);
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
    }
    break;
  case InstORNp:
    pushb(&stackb, InstORNp, data->accumulator);
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
    }
    break;
  case InstXORp:
    pushb(&stackb, InstXORp, data->accumulator);
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
    }
    break;
  case InstXORNp:
    pushb(&stackb, InstXORNp, data->accumulator);
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
    data->accumulator = data->accumulator == 0 ? 1 : 0;
    break;
  case InstNOTp:
    // TODO: add stack support
    // Not included in IEC61131-3
    break;
  case Instq:
    popb(&stackb, &poppedElement);
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
    }

  // TODO: add more instructions here, ADD, SUB, MUL, DIV, GT, GE, EQ, NE, LT,
  // LE, CTU, CTD, TON, TOF etc
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
