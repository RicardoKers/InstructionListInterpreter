#ifndef RLE_INCLUDED
#define RLE_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t encodeRLE(uint8_t *dataIn, uint8_t *dataOut, size_t size);
void decodeRLE(uint8_t *dataIn, uint8_t *dataOut, size_t size);

#endif // VMPARAMETERS_H_INCLUDED