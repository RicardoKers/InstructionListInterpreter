/* This program implements RLE compression for binary data
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "zrle.h"

// Function to generate pseudo-random 8-bit values with sequences of zeros
void generate_random(uint8_t *data, size_t size) {
  // Seed for pseudo-random number generation
  srand(time(NULL));
  
  size_t i = 0;
  int r=0;
  uint8_t v;
  while (i < size) {
    v = rand() % 255;
    if(i>99 && i<401)
    {
      v = 0;
      data[i++] =  v;
    }
    else
    {
      data[i++] =  v;
      r = rand() % 10; // Repetitions from 0 to 9
      for (size_t j = 0; j <= r; j++) {
        data[i++] = v;
      if( i >= size) break;
      }
    }    
  }
}


///////////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////////
int main() {
  // Define the data to compress
  size_t size = 100000;
  uint8_t *dataIn = (uint8_t *)malloc(size);
  if (dataIn == NULL) {
  printf("Error: could not allocate memory for data\n");
  return 1;
  }
  uint8_t *dataOut = (uint8_t *)malloc(size);
  if (dataOut == NULL) {
  printf("Error: could not allocate memory for data\n");
  return 1;
  }
  uint8_t *dataOut2 = (uint8_t *)malloc(size);
  if (dataOut2 == NULL) {
  printf("Error: could not allocate memory for data\n");
  return 1;
  }

  generate_random(dataIn, size);
  
  // print data in hex
  for (size_t i = 0; i < size; i++) {
    //printf("%02x ", dataIn[i]);
  }

  size_t s=encodeZRLE(dataIn, dataOut, size);

  // print data in hex
  printf("\n");
  printf("Encoded data: s = %d\n", s);
  for (size_t i = 0; i < s; i++) {
    //printf("%02x ", dataOut[i]);
  }

  decodeZRLE(dataOut, dataOut2, s);

  // print data in hex
  printf("\n");
  printf("Decoded data:\n");
  for (size_t i = 0; i < size; i++) {
    //printf("%02x ", dataOut2[i]);
  }

  // verify if the decoded data is equal to the original data
  printf("\n");
  printf("\n");
  for (size_t i = 0; i < size; i++) {
    if (dataIn[i] != dataOut2[i]) {
      printf("Error: dataIn[%d] = %x != dataOut2[%d] = %x\n", i, dataIn[i], i, dataOut2[i]);
      return 1;
    }
  
  }
  printf("\n SEM Erros\n");
  return 0;
}
