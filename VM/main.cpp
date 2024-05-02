#include "VM.h"
#include "stack.h"
#include "timer.h"

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
    } else if (instr.operands[i].memorytype == B) {
      if (instr.operands[i].registertype == I)
        printf("IB%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == Q)
        printf("QB%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == M)
        printf("MB%d ", instr.operands[i].address);
    } else if (instr.operands[i].memorytype == W) {
      if (instr.operands[i].registertype == I)
        printf("IW%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == Q)
        printf("QW%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == M)
        printf("MW%d ", instr.operands[i].address);
    } else if (instr.operands[i].memorytype == D) {
      if (instr.operands[i].registertype == I)
        printf("ID%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == Q)
        printf("QD%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == M)
        printf("MD%d ", instr.operands[i].address);
    } else if (instr.operands[i].memorytype == L) {
      if (instr.operands[i].registertype == I)
        printf("IL%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == Q)
        printf("QL%d ", instr.operands[i].address);
      else if (instr.operands[i].registertype == M)
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
    printf("%d\t", i);
  }
  printf("\nI:\t");
  for (uint16_t i = 0; i < InputSize; i++) {
    printf("%X\t", data->Inputs[i]);
  }
  printf("\nM:\t");
  for (uint16_t i = 0; i < MemorySize; i++) {
    printf("%X\t", data->Memories[i]);
  }
  printf("\n0:\t");
  for (uint16_t i = 0; i < OutputSize; i++) {
    printf("%X\t", data->Outputs[i]);
  }
  printf("\nAccumulator = %d", data->accumulator);
  printf("\n-------------------------------------------------------------------"
         "--------------------\n");
}

// append the sum of all bytes of the program to the end of the program in 32 bits format
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

void printProgramInHEX(uint8_t *program, uint16_t size) {
  printf("\n{");
  for (uint16_t i = 0; i < size; i++) {
    printf("0x%02X", program[i]);
    if (i < size - 1)
      printf(",");
  }
  printf("}\n");
}

int main() {
  // Stack initalization

  initStackb(&stackb);

  uint8_t program[1000];
  Data data;
  uint16_t bufPos = 2;
  uint16_t programSize = 0;
  initializeMemory(&data);
  // Input test data
  data.Inputs[0] = 0b00010111;
  data.Inputs[1] = 0b00000000;
  data.Inputs[2] = 0b00000001;
  // Test program
  // LD IX0.0
  Operand operand[10];
  operand[0] = {X, I, 1, 0};
  operand[1] = {X, I, 0, 0};
  operand[2] = {X, I, 0, 0};
  uint64_t Kn[10];
  /*
  Kn[0] = 0x0000000000000000;
  Kn[1] = 0x0000000000000000;
  Kn[2] = 0x0000000000000000;
  
  bufPos = encodeInstruction(program, bufPos, InstLD, operand, Kn);
  // AND IX0.1
  operand[0] = {X, I, 1, 0};
  bufPos = encodeInstruction(program, bufPos, InstAND, operand, Kn);
  // ANDN IX0.2
  operand[0] = {X, I, 2, 0};
  bufPos = encodeInstruction(program, bufPos, InstANDN, operand, Kn);
  // OR IX0.3
  operand[0] = {X, I, 3, 0};
  bufPos = encodeInstruction(program, bufPos, InstOR, operand, Kn);
  // ST QX0.0
  operand[0] = {X, Q, 0, 0};
  bufPos = encodeInstruction(program, bufPos, InstST, operand, Kn);
  // AND( IX0.3
  operand[0] = {X, I, 3, 0};
  bufPos = encodeInstruction(program, bufPos, InstANDp, operand, Kn);
  // OR IX0.4
  operand[0] = {X, I, 4, 0};
  bufPos = encodeInstruction(program, bufPos, InstOR, operand, Kn);
  // )
  bufPos = encodeInstruction(program, bufPos, Instq, operand, Kn);*/
  // LD IX0.0
  operand[0] = {X, I, 0, 0};
  bufPos = encodeInstruction(program, bufPos, InstLD, operand, Kn);
  // mov KX1 MX0.0
  operand[0] = {D, K, 0, 0};
  operand[1] = {L, M, 0, 0};
  Kn[0] = 0x0102030405060708;
  bufPos = encodeInstruction(program, bufPos, InstMOV, operand, Kn);

  programSize = bufPos; // including the 2 bytes for the program size
  program[0] = (uint8_t)(programSize >> 8) & 0xFF;
  program[1] = (uint8_t)programSize & 0xFF;

  encodeProgramCS(program);

  // Run the program
  if(verifyProgramIntegrity(program) != noError) {
    printf("Program integrity error\n");
    return 1;
  }
  bufPos = 2;
  printMemory(&data);
  programSize = getProgramSize(program);
  while (bufPos < programSize) {
    Instruction instr = readInstruction(program, &bufPos);
    printInstruction(instr);
    executeInstruction(program, instr, &data);
    printMemory(&data);
  }
  printProgramInHEX(program, programSize+4);
  printf("Size = %d\n", programSize);
  /*
  // value contersion test
  bufPos=0;
 

  data.Inputs[2]= 0x3f;
  data.Inputs[3]= 0x8c;
  data.Inputs[4]= 0xcc;
  data.Inputs[5]= 0xcd;
  data.Inputs[6]= 0x01;
  data.Inputs[7]= 0x01;
  data.Inputs[8]= 0x01;
  data.Inputs[9]= 0x01;

  operand1 = {W, I, 0, 2}; //  memorytype; registertype; bitNumber; address;

  printf("Value %d\n",operandValueToInt16(&operand1, program, &data));
  */
  //getchar();

  return 0;
}
