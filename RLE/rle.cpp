#include "rle.h"

/** Function to encode data with RLE
 *  @param dataIn: pointer to input data
 *  @param dataOut: pointer to output data
 *  @param size: size of input data
 *  @return size of output data
 */
size_t encodeRLE(uint8_t *dataIn, uint8_t *dataOut, size_t size) {
  size_t i = 0;
  size_t j = 0;
  uint16_t count = 1;
  while (i < size) {
    if (i + 1 < size && dataIn[i] == dataIn[i+1] && count < 255) {
      count++;
    } else {
      dataOut[j++] = count;
      dataOut[j++] = dataIn[i];
      count = 1;
    }
    i++;
  }
  return j;    
}


/** Function to decode data with RLE
 *  @param dataIn: pointer to input data
 *  @param dataOut: pointer to output data
 *  @param size: size of input data
 */
void decodeRLE(uint8_t *dataIn, uint8_t *dataOut, size_t size) {
  size_t i = 0;
  size_t j = 0;
  while (i < size) {
    for (size_t k = 0; k < dataIn[i]; k++) {
      dataOut[j++] = dataIn[i+1];
    }
    i += 2;
  }   
}