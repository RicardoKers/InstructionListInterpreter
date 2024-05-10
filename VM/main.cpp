#include "VM.h"
#include "stack.h"
#include "timer.h"
#include "counter.h"
#include "trigger.h"

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
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    printf("Error opening file %s\n", filename);
    return criticalError;
  }
  uint16_t i = 0;
  int c;
  while ((c = fgetc(file)) != EOF) {
    buffer[i] = (uint8_t)c;
    i++;
  }
  fclose(file);
  return noError;
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
  printf("\nQ:\t");
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

/**
 * Reads the inputs from a file.
 * 
 * @param data The data structure to read the inputs into.
 * @param filename The name of the file to read the inputs from.
*/
void readInputsfromFile(Data *data, const char *filename) {
  FILE *file = fopen(filename, "r");
  unsigned int tmp;
  if (file == NULL) {
    printf("Error opening file %s\n", filename);
    return;
  }
  for (uint16_t i = 0; i < InputSize; i++) {
    fscanf(file, "%X", &tmp);
    data->Inputs[i]=(uint8_t)tmp;
  }
  fclose(file);
}

int main() {
  // debug data + timers + counters + triggers in bytes
  uint8_t debugData[sizeof(Data) + MAX_TIMERS * sizeof(Timer) + MAX_COUNTERS * sizeof(Counter) + MAX_TRIGGERS * sizeof(Trigger) + sizeof(Stack)];
  // Stack initalization
  Stack stack;
  initStack(&stack);

  // timer initialization
  Timer timers[MAX_TIMERS];
  initializeTimer(timers, sizeof(timers)/ sizeof(*timers));

  // counter initialization
  Counter counters[MAX_COUNTERS];
  initializeCounter(counters, sizeof(counters)/ sizeof(*counters));

  // trigger initialization
  Trigger triggers[MAX_TRIGGERS];
  initializeTrigger(triggers, sizeof(triggers)/ sizeof(*triggers));

  ///////////////////////////////////////////////////////////////////////////////////////
  // Testing
  /////////////////////////////////////////////////////////////////////////////////////// 

  // #define Prati
  #define Kerschbaumer 
  
  #ifdef Kerschbaumer
  const char *filename = "..//VMcompiler//program.bin";
  // dynamically allocate a buffer to store the program
  uint32_t fileSize = getProgramSizeFromFile(filename);
  uint8_t *program = (uint8_t *)malloc(fileSize);
  if (program == NULL) {
    printf("Error allocating memory for the program\n");
    return 0;
  }

  // read the program from the file
  if (readProgramFromFile(filename, program) != noError) {
    printf("Error reading the program from file\n");
    return 0;
  }
  #endif // End of Kerschbaumer

  Data data;
  uint16_t bufPos = 2;
  uint16_t programSize = 0;
 
  initializeMemory(&data,timers,counters,triggers,&stack);

  #ifdef Prati
  uint8_t program[1000];// = (uint8_t *)malloc(fileSize);
    
  // Test program
  // LD IX0.0
  Operand operand[10];
  operand[0] = {X, I, 1, 0};
  operand[1] = {X, I, 0, 0};
  operand[2] = {X, I, 0, 0};
  uint64_t Kn[10];

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
  // OR( IX0.5
  operand[0] = {X, I, 5, 0};
  bufPos = encodeInstruction(program, bufPos, InstORp, operand, Kn);
  // OR IX0.4
  operand[0] = {X, I, 4, 0};
  bufPos = encodeInstruction(program, bufPos, InstOR, operand, Kn);
  // )
  bufPos = encodeInstruction(program, bufPos, Instq, operand, Kn);
  // LD IX0.0
  operand[0] = {X, I, 0, 0};
  bufPos = encodeInstruction(program, bufPos, InstLD, operand, Kn);
/*
  // RTRIGGER
  operand[0] = {B, K, 0, 0};
  Kn[0]=2;
  operand[1] = {X, I, 7, 0};
  operand[2] = {X, Q, 7, 3};
  bufPos = encodeInstruction(program, bufPos, InstRTRIGGER, operand, Kn);
*/  
  // mov KX1 MX0.0
  operand[0] = {R, K, 0, 0};
  operand[1] = {R, M, 0, 0};
  Kn[0] = 0x4608f47e; // 8765.1234
  bufPos = encodeInstruction(program, bufPos, InstMOV, operand, Kn);
    
  /*  // Timer (k2,MX8.3,MW2,K1,Q0.3)
    operand[0] = {B, K, 0, 0};
    Kn[0]=2;
    operand[1] = {X, I, 0, 0};
    operand[2] = {W, M, 0, 2};
    Kn[2]=10;
    operand[3] = {B, K, 0, 0};
    Kn[3]=3;
    operand[4] = {X, Q, 3, 0};
    operand[5] = {W, M, 0, 6};
        
    bufPos = encodeInstruction(program, bufPos, InstTON, operand, Kn);
    bufPos = encodeInstruction(program, bufPos, InstTON, operand, Kn);
    bufPos = encodeInstruction(program, bufPos, InstTON, operand, Kn);
    bufPos = encodeInstruction(program, bufPos, InstTON, operand, Kn);
    bufPos = encodeInstruction(program, bufPos, InstTON, operand, Kn);
    bufPos = encodeInstruction(program, bufPos, InstTON, operand, Kn);
  */
/*  
    //counter (k3, IX0.0, k4,IX0.7,q0.6)
    operand[0] = {B, K, 0, 0};
    Kn[0]=2;
    operand[1] = {X, I, 0, 0};
    operand[2] = {W, M, 0, 2};
    Kn[2]=4;
    operand[4] = {X, Q, 6, 0};
    operand[5] = {W, M, 0, 6};

  //Load operation
    operand[3] = {X, I, 0, 0};
    bufPos = encodeInstruction(program, bufPos, InstCTD, operand, Kn);
    operand[3] = {X, I, 7, 0};

   operand[1] = {X, I, 0, 0};
    bufPos = encodeInstruction(program, bufPos, InstCTD, operand, Kn);
   operand[1] = {X, I, 7, 0};
    bufPos = encodeInstruction(program, bufPos, InstCTD, operand, Kn);
*/
  
  
  programSize = bufPos; // including the 2 bytes for the program size
  program[0] = (uint8_t)(programSize >> 8) & 0xFF;
  program[1] = (uint8_t)programSize & 0xFF;

  encodeProgramCS(program); 
  
   // Set the inputs
  data.Inputs[0] = 0b00001111;
  data.Inputs[1] = 0b00000001;
  data.Inputs[2] = 0b00000000;
  
  #endif // End of Prati
  
  // Run the program
  if(verifyProgramIntegrity(program) != noError) {
    printf("Program integrity error\n");
    return 1;
  }
  
  printMemory(&data);
  programSize = getProgramSize(program);
  int c=0;

  while (c != 'q')
  {
    bufPos = 2;
    data.accumulator = 0;    

    #ifdef Kerschbaumer
      readInputsfromFile(&data, "inputs.txt");
    #endif // End of Kerschbaumer

    while (bufPos < programSize) {
      Instruction instr = readInstruction(program, &bufPos);
      printInstruction(instr, program);
      executeInstruction(program, instr, &data);
      printMemory(&data);
      }
    printf("Press 'q <enter>' to quit, or '<enter>' to continue\n");
    printf("######################################################################\n");
    c = getchar();
  }
   
  //printProgramInHEX(program, programSize+4);
  //printf("Size = %d\n", programSize);
  //free(program);
  //getchar();
  return 0;
}