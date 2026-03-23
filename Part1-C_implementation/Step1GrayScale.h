#ifndef GRAYSCALE_H
#define GRAYSCALE_H

#include "cbmp.h"

void toGrayScale(unsigned char input_array[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], 
               unsigned char output_array[BMP_WIDTH][BMP_HEIGTH]);

#endif