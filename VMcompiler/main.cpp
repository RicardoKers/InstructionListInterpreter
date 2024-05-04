// Este programa serve para ler um arquivo de texto com instruções de um programa em linguagem de 
// máquina e transformar em um arquivo binário. O arquivo binário é composto por um cabeçalho de 2 bytes indicando o tamanho do programa, seguido das instruções e por fim um checksum de 4 bytes.
// O programa é composto por instruções de 0 ou mais operandos, onde cada operando é composto por 3 bytes, sendo o primeiro byte composto
// pelo tipo de memória (3 bits), o tipo de registrador (2 bits) e o número do bit (3 bits). Os outros 2 bytes são o endereço do operando para
// os tipos de registradores I, Q, M e o valor do operando para o tipo de registrador K, neste caso com tamanho de 1 a 8 bytes.
// O arquivo a ser lido deve conter as instruções em texto, onde cada instrução é composta pelo nome da instrução seguido dos operandos 
// separados por espaço. Por exemplo:
// LD IX1.0, que carrega o bit 0 do registrador de entrada 1.
// AND IX1.0 MX0.0, que faz a operação AND entre o bit 0 do registrador de entrada 1 e o bit 0 do registrador de memória 0.
// MOV KX1 MX0.0, que move o valor constante 1 para o bit 0 do registrador de memória 0.
// O programa permite comentários iniciados por #.
// O programa deve ser capaz de ler o arquivo de texto, transformar as instruções em instruções binárias e salvar em um arquivo binário.

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
  case InstMOVp:
      return NumOpMOVp;
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
  case InstNOTp:
      return NumOpNOTp;
    break;
  case InstADD:
      return NumOpADD;
    break;
  case InstADDp:
      return NumOpADDp;
    break;
  case InstSUB:
      return NumOpSUB;
    break;
  case InstSUBp:
      return NumOpSUBp;
    break;
  case InstMUL:
      return NumOpMUL;
    break;
  case InstMULp:
      return NumOpMULp;
    break;
  case InstDIV:
      return NumOpDIV;
    break;
  case InstDIVp:
      return NumOpDIVp;
    break;
  case InstGT:
      return NumOpGT;
    break;
  case InstGTp:
      return NumOpGTp;
    break;
  case InstGE:
      return NumOpGE;
    break;
  case InstGEp:
      return NumOpGEp;
    break;
  case InstEQ:
      return NumOpEQ;
    break;
  case InstEQp:
      return NumOpEQp;
    break;
  case InstNE:
      return NumOpNE;
    break;
  case InstNEp:
      return NumOpNEp;
    break;
  case InstLT:
      return NumOpLT;
    break;
  case InstLTp:
      return NumOpLTp;
    break;
  case InstLE:
      return NumOpLE;
    break;
  case InstLEp:
      return NumOpLEp;
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
  case Instq:
      return NumOpq;
  default:
      return 0;
    break;
  }
  return 0;
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
  while (buffer[*pos] == ' ' || buffer[*pos] == '\n' || buffer[*pos] == '\t') {
    (*pos)++;
  }
  
  if(buffer[*pos] == '#') {
    while (buffer[*pos] != '\n' && buffer[*pos] != '\0') {
      (*pos)++;
    }
  }

  while (buffer[*pos] == ' ' || buffer[*pos] == '\n' || buffer[*pos] == '\t') {
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
      printf("Invalid number %s\n", token);
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
    printf("Invalid register type %c\n", token[i]);
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
      printf("Invalid memory type %c\n", token[i]);
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
            printf("Invalid bit number %d\n", tmp);
            return criticalError;
          }
        }
      } else {
        printf("Invalid token %s\n", token);
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
      printf("Invalid memory type %c\n", token[i]);
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
    printf("Invalid instruction %s\n", token);
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
  }
  if(num_operands == 3) {
    if(inst->operands[0].memorytype != inst->operands[1].memorytype ||
       inst->operands[0].memorytype != inst->operands[2].memorytype ||
       inst->operands[1].memorytype != inst->operands[2].memorytype) {
      printf("\tWarning: operands with different memory types\n");
      ret = warning;
    }
  }
  return ret;
  // TODO: implement more checks
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
      buffer[bufPos + 1] = (uint8_t)(operand[i].address >> 8) & 0xFF;
      buffer[bufPos + 2] = (uint8_t)operand[i].address & 0xFF;
      bufPos += 3;
    } else {
      if(operand[i].memorytype == X) {
        buffer[bufPos + 1] = (uint8_t)Kn[i] & 0xFF;
        bufPos += 2;
      } else if(operand[i].memorytype == B) {
        buffer[bufPos + 1] = (uint8_t)Kn[i] & 0xFF;
        bufPos += 2;
      } else if(operand[i].memorytype == W) {
        buffer[bufPos + 1] = (uint8_t)(Kn[i] >> 8) & 0xFF;
        buffer[bufPos + 2] = (uint8_t)Kn[i] & 0xFF;
        bufPos += 3;
      } else if(operand[i].memorytype == D) {
        buffer[bufPos + 1] = (uint8_t)(Kn[i] >> 24) & 0xFF;
        buffer[bufPos + 2] = (uint8_t)(Kn[i] >> 16) & 0xFF;
        buffer[bufPos + 3] = (uint8_t)(Kn[i] >> 8) & 0xFF;
        buffer[bufPos + 4] = (uint8_t)Kn[i] & 0xFF;
        bufPos += 5;
      } else if(operand[i].memorytype == L) {
        buffer[bufPos + 1] = (uint8_t)(Kn[i] >> 56) & 0xFF;
        buffer[bufPos + 2] = (uint8_t)(Kn[i] >> 48) & 0xFF;
        buffer[bufPos + 3] = (uint8_t)(Kn[i] >> 40) & 0xFF;
        buffer[bufPos + 4] = (uint8_t)(Kn[i] >> 32) & 0xFF;
        buffer[bufPos + 5] = (uint8_t)(Kn[i] >> 24) & 0xFF;
        buffer[bufPos + 6] = (uint8_t)(Kn[i] >> 16) & 0xFF;
        buffer[bufPos + 7] = (uint8_t)(Kn[i] >> 8) & 0xFF;
        buffer[bufPos + 8] = (uint8_t)Kn[i] & 0xFF;
        bufPos += 9;
      } else if(operand[i].memorytype == R) {
        buffer[bufPos + 1] = (uint8_t)(Kn[i] >> 24) & 0xFF;
        buffer[bufPos + 2] = (uint8_t)(Kn[i] >> 16) & 0xFF;
        buffer[bufPos + 3] = (uint8_t)(Kn[i] >> 8) & 0xFF;
        buffer[bufPos + 4] = (uint8_t)Kn[i] & 0xFF;
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
    printf("MOV( ");
    break;
  case InstAND:
    printf("AND ");
    break;
  case InstANDp:
    printf("AND( ");
    break;
  case InstANDN:
    printf("ANDN ");
    break;
  case InstANDNp:
    printf("ANDN( ");
    break;
  case InstOR:
    printf("OR ");
    break;
  case InstORp:
    printf("OR( ");
    break;
  case InstORN:
    printf("ORN ");
    break;
  case InstORNp:
    printf("ORN( ");
    break;
  case InstXOR:
    printf("XOR ");
    break;
  case InstXORp:
    printf("XOR( ");
    break;
  case InstXORN:
    printf("XORN ");
    break;
  case InstXORNp:
    printf("XORN( ");
    break;
  case InstNOT:
    printf("NOT ");
    break;
  case InstNOTp:
    printf("NOT( ");
    break;
  case InstADD:
    printf("ADD ");
    break;
  case InstADDp:
    printf("ADD( ");
    break;
  case InstSUB:
    printf("SUB ");
    break;
  case InstSUBp:
    printf("SUB( ");
    break;
  case InstMUL:
    printf("MUL ");
    break;
  case InstMULp:
    printf("MUL( ");
    break;
  case InstDIV:
    printf("DIV ");
    break;
  case InstDIVp:
    printf("DIV( ");
    break;
  case InstGT:
    printf("GT ");
    break;
  case InstGTp:
    printf("GT( ");
    break;
  case InstGE:
    printf("GE ");
    break;
  case InstGEp:
    printf("GE( ");
    break;
  case InstEQ:
    printf("EQ ");
    break;
  case InstEQp:
    printf("EQ( ");
    break;
  case InstNE:
    printf("NE ");
    break;
  case InstNEp:
    printf("NE( ");
    break;
  case InstLT:
    printf("LT ");
    break;
  case InstLTp:
    printf("LT( ");
    break;
  case InstLE:
    printf("LE ");
    break;
  case InstLEp:
    printf("LE( ");
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
  case Instq:
    printf(") ");
    break;
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
        printf("KW%d ", (int16_t)(program[instr.operands[i].address]
                        <<8 | program[instr.operands[i].address+1]));
    } else if (instr.operands[i].memorytype == D) {
      if (instr.operands[i].registertype == I)
        printf("ID%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == Q)
        printf("QD%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == M)
        printf("MD%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == K)
        printf("KD%d ", (int32_t)(program[instr.operands[i].address]
                        <<24 | program[instr.operands[i].address+1]<<16 |
                        program[instr.operands[i].address+2]<<8 |
                        program[instr.operands[i].address+3]));
    } else if (instr.operands[i].memorytype == L) {
      if (instr.operands[i].registertype == I)
        printf("IL%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == Q)
        printf("QL%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == M)
        printf("ML%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == K)
        printf("KD%d ", (int64_t)((uint64_t)program[instr.operands[i].address]
                        <<56 | (uint64_t)program[instr.operands[i].address+1]<<48 |
                        (uint64_t)program[instr.operands[i].address+2]<<40 |
                        (uint64_t)program[instr.operands[i].address+3]<<32 |
                        program[instr.operands[i].address+4]<<24 |
                        program[instr.operands[i].address+5]<<16 |
                        program[instr.operands[i].address+6]<<8 |
                        program[instr.operands[i].address+7]));
    }
    else if (instr.operands[i].memorytype == R) {
      if (instr.operands[i].registertype == I)
        printf("IR%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == Q)
        printf("QR%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == M)
        printf("MR%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == K){
        uint32_t temp = (program[instr.operands[i].address]
                        <<24 | program[instr.operands[i].address+1]<<16 |
                        program[instr.operands[i].address+2]<<8 |
                        program[instr.operands[i].address+3]);
        printf("KR%f ",*(float *)&temp);
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
  return (buffer[0] << 8) | buffer[1];
}

/**
 * Encodes the size of the program into a buffer.
 *
 * @param buffer The buffer to encode the size into.
 * @param size The size of the program.
 */
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
  program[size] = (uint8_t)(sum >> 24) & 0xFF;
  program[size + 1] = (uint8_t)(sum >> 16) & 0xFF;
  program[size + 2] = (uint8_t)(sum >> 8) & 0xFF;
  program[size + 3] = (uint8_t)sum & 0xFF;
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


///////////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////////
int main() {

  // dynamically allocate a buffer to store the program
  uint16_t programSize = getProgramSizeFromFile("program.txt");
  uint8_t *program = (uint8_t *)malloc(programSize);
  if (program == NULL) {
    printf("Error allocating memory for the program\n");
    return 0;
  }

  // read the program from the file
  if (readProgramFromFile("program.txt", program) != noError) {
    printf("Error reading the program from file\n");
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
  printf("\n");
  while (program[bufPos] != '\0')
  {
    // ignore extra \n at the end of the file
    if(program[bufPos] == '\n'&& program[bufPos+1] == '\0') {
      break;
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
  outBuffer[0] = (uint8_t)(outBufPos >> 8) & 0xFF;
  outBuffer[1] = (uint8_t)outBufPos & 0xFF;

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

  printf("\nCompiled successfully\n");
  return 0;
}
