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
                           uint8_t num_operands, Operand operand0,
                           Operand operand1, Operand operand2) {
  buffer[bufPos] = (opperation << 2) | num_operands;
  bufPos += 1;
  if (num_operands > 0) {
    buffer[bufPos] = operand0.memorytype << 5 | operand0.registertype << 3 |
                     operand0.bitNumber;
    buffer[bufPos + 1] = (uint8_t)(operand0.address >> 8) & 0xFF;
    buffer[bufPos + 2] = (uint8_t)operand0.address & 0xFF;
    bufPos += 3;
  }
  if (num_operands > 1) {
    buffer[bufPos] = operand1.memorytype << 5 | operand1.registertype << 3 |
                     operand1.bitNumber;
    buffer[bufPos + 1] = (uint8_t)(operand1.address >> 8) & 0xFF;
    buffer[bufPos + 2] = (uint8_t)operand1.address & 0xFF;
    bufPos += 3;
  }
  if (num_operands > 2) {
    buffer[bufPos] = operand2.memorytype << 5 | operand2.registertype << 3 |
                     operand2.bitNumber;
    buffer[bufPos + 1] = (uint8_t)(operand2.address >> 8) & 0xFF;
    buffer[bufPos + 2] = (uint8_t)operand2.address & 0xFF;
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
  printf("\n-------------------------------------------------------------------"
         "--------------------\n");
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

  // Test program
  // LD IX0.0
  Operand operand1 = {X, I, 0, 0};
  Operand operand2 = {X, I, 0, 0};
  Operand operand3 = {X, I, 0, 0};
  bufPos = encodeInstruction(program, bufPos, InstLD, 1, operand1, operand2,
                             operand3);
  // AND IX0.1
  operand1 = {X, I, 1, 0};
  bufPos = encodeInstruction(program, bufPos, InstAND, 1, operand1, operand2,
                             operand3);
  // ANDN IX0.2
  operand1 = {X, I, 2, 0};
  bufPos = encodeInstruction(program, bufPos, InstANDN, 1, operand1, operand2,
                             operand3);
  // OR IX0.3
  operand1 = {X, I, 3, 0};
  bufPos = encodeInstruction(program, bufPos, InstOR, 1, operand1, operand2,
                             operand3);
  // ST QX0.0
  operand1 = {X, Q, 0, 0};
  bufPos = encodeInstruction(program, bufPos, InstST, 1, operand1, operand2,
                             operand3);

  // AND( IX0.3
  operand1 = {X, I, 3, 0};
  bufPos = encodeInstruction(program, bufPos, InstANDp, 1, operand1, operand2,
                             operand3);

  // OR IX0.4
  operand1 = {X, I, 4, 0};
  bufPos = encodeInstruction(program, bufPos, InstOR, 1, operand1, operand2,
                             operand3);

  // )
  bufPos = encodeInstruction(program, bufPos, Instq, 0, operand1, operand2,
                             operand3);

  programSize = bufPos; // including the 2 bytes for the program size
  program[0] = (uint8_t)(programSize >> 8) & 0xFF;
  program[1] = (uint8_t)programSize & 0xFF;

  // Run the program
  bufPos = 2;
  printMemory(&data);
  while (bufPos < getProgramSize(program)) {
    Instruction instr = readInstruction(program, &bufPos);
    printInstruction(instr);
    executeInstruction(instr, &data);
    //printMemory(&data);
  }
  printProgramInHEX(program, programSize);
  printf("PSize = %d\n", programSize);
  getchar();

  return 0;
}
