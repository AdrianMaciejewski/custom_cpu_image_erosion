#ifndef ALL_RESULTS_H
#define ALL_RESULTS_H

#include "cbmp.h"   // for BMP_WIDTH, BMP_HEIGTH, BMP_CHANNELS

// Arrays produced by the processing pipeline
extern unsigned char original[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
extern unsigned char grayscale[BMP_WIDTH][BMP_HEIGTH];
extern unsigned char binary[BMP_WIDTH][BMP_HEIGTH];
extern unsigned char marked[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];

extern int chords[][2];
extern int nChords;

#endif // ALL_RESULTS_H
