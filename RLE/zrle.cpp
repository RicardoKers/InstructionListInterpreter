#include "zrle.h"

/** Function to encode zeros with ZRLE
 *  @param dataIn: pointer to input data
 *  @param dataOut: pointer to output data
 *  @param size: size of input data
 *  @return size of output data
 */
size_t encodeZRLE(uint8_t *dataIn, uint8_t *dataOut, size_t size) {
    size_t i = 0;
    size_t j = 0;
    uint16_t count = 0;
    while (i < size) {
        if (count > 0 && dataIn[i] != 0) {
            dataOut[j++] = 0;
            dataOut[j++] = count;
            count = 0;
        }
        if (dataIn[i] == 0) {
            count++;
            if (count == 255) {
                dataOut[j++] = 0;
                dataOut[j++] = count;
                count = 0;
            }
        } else {
            dataOut[j++] = dataIn[i];
        }
        i++;
    }
    if (count > 0) {
        dataOut[j++] = 0;
        dataOut[j++] = count;
    }
    return j;    
}

/** Function to decode data with ZRLE
 *  @param dataIn: pointer to input data
 *  @param dataOut: pointer to output data
 *  @param size: size of input data
 */
void decodeZRLE(uint8_t *dataIn, uint8_t *dataOut, size_t size) {
  size_t i = 0;
  size_t j = 0;
  while (i < size) {
    if (dataIn[i] == 0) {
      i++;
      for (size_t k = 0; k < dataIn[i]; k++) {
        dataOut[j++] = 0;
      }
    } else {
        dataOut[j++] = dataIn[i];
    }
    i++;
  }   
}