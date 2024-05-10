/* This program is used to read a text file with machine language instructions and convert 
it into a binary file. The binary file consists of a 2-byte header indicating the size of 
the program, followed by the instructions, and finally a 4-byte checksum. The program is 
composed of instructions with 0 or more operands, where each operand is composed of 3 bytes. 
The first byte represents the memory type (3 bits), register type (2 bits), and bit number (3 bits). 
The other 2 bytes represent the operand address for register types I, Q, M, and the operand value 
for register type K, which can range from 1 to 8 bytes. The input file should contain the instructions 
in text format, where each instruction consists of the instruction name followed by the operands 
separated by spaces. For example:
LD IX1.0, which loads bit 0 of input register 1.
AND IX1.0 MX0.0, which performs the AND operation between bit 0 of input register 1 and bit 0 of memory register 0.
MOV KX1 MX0.0, which moves the constant value 1 to bit 0 of memory register 0.
The program allows comments starting with #.
The program should be able to read the text file, convert the instructions into binary instructions, 
and save them into a binary file.
*/

#include "VMCompiler.h"

/**
 * Gets the size of a file.
 * 
 * @param filename The name of the file to get the size of.
 * @return The size of the file.
*/
uint32_t getProgramSizeFromFile(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error opening file %s\n", filename);
    return 0;
  }
  uint32_t size = 0;
  while (!feof(file)) {
    fgetc(file);
    size++;
  }
  fclose(file);
  return size;
}

/**
 * Reads a program from a file.
 * 
 * @param filename The name of the file to read the program from.
 * @param buffer The buffer to read the program into.
 * @return The error code.
*/
uint8_t readProgramFromFile(const char *filename, uint8_t *buffer) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error opening file %s\n", filename);
    return criticalError;
  }
  uint16_t i = 0;
  while (!feof(file)) {
    buffer[i] = fgetc(file);
    i++;
  }
  fclose(file);
  return noError;
}

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
 * Gets the next token from a buffer.
 *
 * @param pos The position in the buffer to get the token from.
 * @param buffer The buffer to get the token from.
 * @param token The token to store the result in.
 */
void getNextToken(uint32_t *pos, uint8_t *buffer, char *token) {
  uint8_t i = 0;
  while (buffer[*pos] == ' ' || buffer[*pos] == '\n' || buffer[*pos] == '\t' || buffer[*pos] == '#') {
    if(buffer[*pos] == '#') {
      while (buffer[*pos] != '\n' && buffer[*pos] != '\0') {
        (*pos)++;
      }
    }
    (*pos)++;
  }
    
  while (buffer[*pos] != ' ' && buffer[*pos] != '\n' && buffer[*pos] != '\t' && buffer[*pos] != '\0') {
    token[i] = buffer[*pos];
    i++;
    (*pos)++;
  }
  token[i] = '\0';
}

/**
 * Gets a 64 bit number from a token.
 * 
 * @param pos The position in the token to get the number from.
 * @param token The token to get the number from.
 * @param value The value to store the result in.
 * @return The error code.
 */
uint8_t get64bitNumberFromToken(uint8_t *pos, char *token, uint64_t *value, uint8_t memorytype) {
  uint64_t temp = 0;
  float tempf = 0;
  if(memorytype == X || memorytype == B || memorytype == W || memorytype == D || memorytype == L) {
    if(token[(*pos+1)] == 'x') // hex number
      sscanf(token+(*pos), "%x", &temp);
    else
      sscanf(token+(*pos), "%d", &temp);
  } else if(memorytype == R) {
    sscanf(token+(*pos), "%f", &tempf);
    temp = *(uint64_t *)&tempf;      
  }
  (*value) = temp;
  return noError;
}

/**
 * Gets a 16 bit number from a token.
 * 
 * @param pos The position in the token to get the number from.
 * @param token The token to get the number from.
 * @param value The value to store the result in.
 * @return The error code.
 */
uint8_t get16bitNumberFromToken(uint8_t *pos, char *token, uint16_t *value) {
  uint16_t temp = 0;
  while (token[(*pos)] != '\0' && token[(*pos)] != '.') {
    if (token[(*pos)] >= '0' && token[(*pos)] <= '9') {
      temp = temp * 10 + (token[(*pos)] - '0');
    } else {
      printf("Error: Invalid number %s\n", token);
      return criticalError;
    }
    (*pos)++;
  }
  (*value) = temp;
  return noError;
}

/**
 * Gets the operand from a token.
 * 
 * @param token The token to get the operand from.
 * @param operand The operand to store the result in.
 * @return The error code.
 */
uint8_t getOperandFromToken(char *token, Operand *operand, uint64_t *Kn) {
  uint16_t tmp = 0;
  uint8_t i = 0;
  (*Kn) = 0;
  if (token[i] == 'I') {
    operand->registertype = I;
  } else if (token[i] == 'Q') {
    operand->registertype = Q;
  } else if (token[i] == 'M') {
    operand->registertype = M;
  } else if (token[i] == 'K') {
    operand->registertype = K;
  } else {
    printf("Error: Invalid register type %c\n", token[i]);
    return criticalError;
  }
  i++;
  if(operand->registertype == K) {
    if (token[i] == 'X') {
      operand->memorytype = X;
    } else if (token[i] == 'B') {
      operand->memorytype = B;
    } else if (token[i] == 'W') {
      operand->memorytype = W;
    } else if (token[i] == 'D') {
      operand->memorytype = D;
    } else if (token[i] == 'L') {
      operand->memorytype = L;
    } else if (token[i] == 'R') {
      operand->memorytype = R;
    } else {
      printf("Error: Invalid memory type %c\n", token[i]);
      return criticalError;
    }
    i++;
    if(get64bitNumberFromToken(&i, token, Kn, operand->memorytype) != noError) {
      return criticalError;
    }
    operand->address = 0;
    return noError;
  } else {
     if (token[i] == 'X') {
      operand->memorytype = X;
      i++;
      if(get16bitNumberFromToken(&i, token, &operand->address) != noError) {
        return criticalError;
      }
      if(token[i] == '.') {
        i++;
        if(get16bitNumberFromToken(&i, token, &tmp) != noError) {
          return criticalError;
        } else {
          if(tmp>=0 && tmp<=7) {
            operand->bitNumber = tmp;
          } else {
            printf("Error: Invalid bit number %d\n", tmp);
            return criticalError;
          }
        }
      } else {
        printf("Error: Invalid token %s\n", token);
        return criticalError;
      }
    } else if (token[i] == 'B') {
      operand->memorytype = B;
      i++;
      if(get16bitNumberFromToken(&i, token, &operand->address) != noError) {
        return criticalError;
      }
    } else if (token[i] == 'W') {
      operand->memorytype = W;
      i++;
      if(get16bitNumberFromToken(&i, token, &operand->address) != noError) {
        return criticalError;
      }
    } else if (token[i] == 'D') {
      operand->memorytype = D;
      i++;
      if(get16bitNumberFromToken(&i, token, &operand->address) != noError) {
        return criticalError;
      }
    } else if (token[i] == 'L') {
      operand->memorytype = L;
      i++;
      if(get16bitNumberFromToken(&i, token, &operand->address) != noError) {
        return criticalError;
      }
    } else if (token[i] == 'R') {
      operand->memorytype = R;
      i++;
      if(get16bitNumberFromToken(&i, token, &operand->address) != noError) {
        return criticalError;
      }
    } else {
      printf("Error: Invalid memory type %c\n", token[i]);
      return criticalError;
    }
    return noError;
  }
}

/**
 * Gets the instruction from a buffer.
 * 
 * @param inst The instruction to get.
 * @param pos The position in the buffer to get the instruction from.
 * @param buffer The buffer to get the instruction from.
 * @return The error code.
 */
 uint8_t getInstruction(Instruction *inst, uint32_t *pos, uint8_t *buffer, uint64_t Kn[]) {
  char token[30]; // buffer to store the token.
  getNextToken(pos, buffer, token);
  uint8_t opcode = 0;
  uint8_t i;
  for (i = 0; i < NumInstructions; i++) {
    if (strcmp(token, InstNames[i]) == 0) {
      opcode = i;
      break;
    }
  }
  // check if the instruction is valid
  if(i==NumInstructions) {
    printf("Error: Invalid instruction %s\n", token);
    return criticalError;
  }
  uint8_t num_operands = getNumOp(opcode);
  inst->opcode = opcode;
  inst->num_operands = num_operands;
  for (i = 0; i < num_operands; i++) {
    getNextToken(pos, buffer, token);
    if(getOperandFromToken(token, &inst->operands[i], &Kn[i]) != noError) {
      return criticalError;
    }
  }
  return noError;
}

/**
 * Verifies if an instruction is valid.
 * 
 * @param inst The instruction to verify.
 * @return The error code.
 */
uint8_t verifyInstruction(Instruction *inst) {
  uint8_t ret=noError;
  uint8_t num_operands = getNumOp(inst->opcode);
  for (uint8_t i = 0; i < num_operands; i++) {
    if(inst->operands[i].registertype == I) {
      if(inst->operands[i].address >= InputSize) {
        printf("Error: Invalid input address %d\n", inst->operands[i].address);
        return criticalError;
      }
    } else if(inst->operands[i].registertype == Q) {
      if(inst->operands[i].address >= OutputSize) {
        printf("Error: Invalid output address %d\n", inst->operands[i].address);
        return criticalError;
      }
    } else if(inst->operands[i].registertype == M) {
      if(inst->operands[i].address >= MemorySize) {
        printf("Error: Invalid memory address %d\n", inst->operands[i].address);
        return criticalError;
      }
    }
  }
  if(num_operands == 2) {
    if(inst->operands[0].memorytype != inst->operands[1].memorytype) {
      printf("\tWarning: operands with different memory types\n");
      ret = warning;
    }
    if((inst->opcode == InstMOV) && inst->operands[1].registertype == K) {
      printf("Error: cannot change a constant value\n");
      return criticalError;
    }
  }
  if(num_operands == 3) {
    if(inst->operands[0].memorytype != inst->operands[1].memorytype ||
       inst->operands[0].memorytype != inst->operands[2].memorytype ||
       inst->operands[1].memorytype != inst->operands[2].memorytype) {
      printf("\tWarning: operands with different memory types\n");
      ret = warning;
    }
    if(inst->operands[2].registertype == K) {
      printf("Error: cannot change a constant value\n");
      return criticalError;
    }
  }
  return ret;
  // TODO: implement more checks
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
uint16_t encodeInstruction(uint8_t *buffer, uint16_t bufPos, uint8_t opperation,
                           Operand operand[], uint64_t Kn[]) {
  buffer[bufPos] = opperation;
  uint8_t num_operands = getNumOp(opperation);
  bufPos += 1;
  for(int i=0; i < num_operands; i++) {
    buffer[bufPos] = operand[i].memorytype << 5 | operand[i].registertype << 3 |
                     operand[i].bitNumber;
    if(operand[i].registertype != K) {
      setWordInAddress(buffer, bufPos + 1, operand[i].address);
      bufPos += 3;
    } else {
      if(operand[i].memorytype == X) {
        buffer[bufPos + 1] = (uint8_t)Kn[i] & 0xFF;
        bufPos += 2;
      } else if(operand[i].memorytype == B) {
        buffer[bufPos + 1] = (uint8_t)Kn[i] & 0xFF;
        bufPos += 2;
      } else if(operand[i].memorytype == W) {
        setWordInAddress(buffer, bufPos + 1, (int16_t)Kn[i]);
        bufPos += 3;
      } else if(operand[i].memorytype == D) {
        setDoubleWordInAddress(buffer, bufPos + 1, (uint32_t)Kn[i]);
        bufPos += 5;
      } else if(operand[i].memorytype == L) {
        setLongWordInAddress(buffer, bufPos + 1, (uint64_t)Kn[i]);
        bufPos += 9;
      } else if(operand[i].memorytype == R) {
        setDoubleWordInAddress(buffer, bufPos + 1, (uint32_t)Kn[i]);
        bufPos += 5;
      }
    }
  }
  
  buffer[bufPos] = 0;
  return bufPos;
}

/**
 * Prints an instruction.
 *
 * @param instr The instruction to print.
 */
void printInstruction(Instruction instr, uint8_t *program) {
  switch (instr.opcode) {
  case InstLD: printf("LD "); break;
  case InstLDN: printf("LDN "); break;
  case InstST: printf("ST "); break;
  case InstSTN: printf("STN "); break;
  case InstS: printf("S "); break;
  case InstR: printf("R "); break;
  case InstMOV: printf("MOV "); break;
  case InstAND: printf("AND "); break;
  case InstANDp: printf("AND( "); break;
  case InstANDN: printf("ANDN "); break;
  case InstANDNp: printf("ANDN( "); break;
  case InstOR: printf("OR "); break;
  case InstORp: printf("OR( "); break;
  case InstORN: printf("ORN "); break;
  case InstORNp: printf("ORN( "); break;
  case InstXOR: printf("XOR "); break;
  case InstXORp: printf("XOR( "); break;
  case InstXORN: printf("XORN "); break;
  case InstXORNp: printf("XORN( "); break;
  case InstNOT: printf("NOT "); break;
  case InstADD: printf("ADD "); break;
  case InstSUB: printf("SUB "); break;
  case InstMUL: printf("MUL "); break;
  case InstDIV: printf("DIV "); break;
  case InstMOD: printf("MOD "); break;
  case InstGT: printf("GT "); break;
  case InstGE: printf("GE "); break;
  case InstEQ: printf("EQ "); break;
  case InstNE: printf("NE "); break;
  case InstLT: printf("LT "); break;
  case InstLE: printf("LE "); break;
  case InstCTU: printf("CTU "); break;
  case InstCTD: printf("CTD "); break;
  case InstTON: printf("TON "); break;
  case InstTOF: printf("TOF "); break;
  case InstTP: printf("TP "); break;
  case InstRTRIGGER: printf("RTRIGGER "); break;
  case InstFTRIGGER: printf("FTRIGGER "); break;
  case Instq: printf(") "); break;
  default:
    break;
  }
  for (uint16_t i = 0; i < instr.num_operands; i++) {
    if (instr.operands[i].memorytype == X) {
      if (instr.operands[i].registertype == I)
        printf("IX%d.%d ", instr.operands[i].address,
               instr.operands[i].bitNumber);
      else if (instr.operands[i].registertype == Q)
        printf("QX%d.%d ", instr.operands[i].address,
               instr.operands[i].bitNumber);
      else if (instr.operands[i].registertype == M)
        printf("MX%d.%d ", instr.operands[i].address,
               instr.operands[i].bitNumber);
      else if (instr.operands[i].registertype == K)
          printf("KX%d ", (program[instr.operands[i].address])==0?0:1);
    } else if (instr.operands[i].memorytype == B) {
      if (instr.operands[i].registertype == I)
        printf("IB%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == Q)
        printf("QB%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == M)
        printf("MB%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == K)
        printf("KB%d ", program[instr.operands[i].address]);
    } else if (instr.operands[i].memorytype == W) {
      if (instr.operands[i].registertype == I)
        printf("IW%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == Q)
        printf("QW%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == M)
        printf("MW%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == K)
        printf("KW%d ", getWordFromAddress(program, instr.operands[i].address));
    } else if (instr.operands[i].memorytype == D) {
      if (instr.operands[i].registertype == I)
        printf("ID%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == Q)
        printf("QD%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == M)
        printf("MD%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == K)
        printf("KD%d ", getDoubleWordFromAddress(program, instr.operands[i].address));
    } else if (instr.operands[i].memorytype == L) {
      if (instr.operands[i].registertype == I)
        printf("IL%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == Q)
        printf("QL%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == M)
        printf("ML%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == K)
        printf("KD%ld ", getLongWordFromAddress(program, instr.operands[i].address));
    }
    else if (instr.operands[i].memorytype == R) {
      if (instr.operands[i].registertype == I)
        printf("IR%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == Q)
        printf("QR%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == M)
        printf("MR%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == K){
        printf("KR%f ",getFloatFromAddress(program, instr.operands[i].address));
      }        
    }
  }
  printf("\n");
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

/**
 * Encodes the program checksum.
 *
 * @param program The program to encode the checksum for.
 */
void encodeProgramCS(uint8_t *program) {
  uint16_t size = getProgramSize(program);
  uint32_t sum = 0;
  for (uint16_t i = 0; i < size; i++) {
    sum += program[i];
  }
  DataUnion u;
  u.u8 = (program + size);
  u.u32[0] = sum;
}

/**
 * Prints a program in HEX format.
 *
 * @param program The program to print.
 * @param size The size of the program.
 */
void printProgramInHEX(uint8_t *program, uint16_t size) {
  printf("\n{");
  for (uint16_t i = 0; i < size; i++) {
    printf("0x%02X", program[i]);
    if (i < size - 1)
      printf(",");
  }
  printf("}\n");
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

///////////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////////
int main() {
  // file name
  const char *filename = "program.il";

  // dynamically allocate a buffer to store the program
  uint16_t programSize = getProgramSizeFromFile(filename);
  uint8_t *program = (uint8_t *)malloc(programSize);
  if (program == NULL) {
    printf("Error: allocating memory for the program\n");
    return 0;
  }

  // read the program from the file
  if (readProgramFromFile(filename, program) != noError) {
    printf("Error: reading the program from file\n");
    return 0;
  }

  program[programSize-1] = '\0'; // add a null terminator to the end of the program
  char token[30]; // buffer to store the token.

  // read the program from the buffer
  uint32_t bufPos = 0;    
  uint16_t testBufPos = 2; // start after the size of the program
  uint8_t outBuffer[10000];
  uint16_t outBufPos = 2; // start after the size of the program
  Instruction instr;
  Instruction testInstr;
  uint64_t Kn[10];
  printf("\nCompiling: %s\n\n", filename);
  while (program[bufPos] != '\0') {
    while(program[bufPos] == ' ' || program[bufPos+1] == '\t' || program[bufPos] == '\n') {
      bufPos++;      
    }
    
    if(program[bufPos] == '\0') break;

    if(program[bufPos] == '#') {
      while(program[bufPos] != '\n') {
        bufPos++;
      }
      continue;
    }
    
    // get the instruction from the buffer
    if(getInstruction(&instr, &bufPos, program, Kn) != noError) {
      return 0;
    }
    
    // encode the instruction into the output buffer
    outBufPos = encodeInstruction(outBuffer, outBufPos, instr.opcode, instr.operands, Kn);
    
    // read the instruction from the output buffer to test the decoding and print it
    testInstr = readInstruction(outBuffer, &testBufPos);
    printInstruction(testInstr, outBuffer); 

     // verify if the instruction is valid
    if(verifyInstruction(&testInstr) == criticalError) {
      return 0;
    }
  }

  // add final size to the output buffer
  DataUnion u;
  u.u8 = outBuffer;
  u.u16[0] = outBufPos;

  // encode the checksum of the program
  encodeProgramCS(outBuffer);

  // save de program to a file
  FILE *file = fopen("program.bin", "wb");
  if (file == NULL) {
    printf("Error opening file program.bin\n");
    return 0;
  }
  fwrite(outBuffer, 1, outBufPos+4, file);
  fclose(file);

  printf("\nCompiled successfully");
  printProgramInHEX(outBuffer, outBufPos+4);
  return 0;
}

