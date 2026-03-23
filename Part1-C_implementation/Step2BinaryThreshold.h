#ifndef BINARY_THRESHOLD_H
#define BINARY_THRESHOLD_H

#include "cbmp.h"

#define THRESHOLD 15

void toBinaryScale(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], int threshold);

#endif