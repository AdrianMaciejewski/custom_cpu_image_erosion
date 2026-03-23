#ifndef MaRKINGCELLS_H
#define MaRKINGCELLS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cbmp.h"


#define X_SIZE 13 // size in px
#define X_COLOR 0xff0000 // red
#define X_THICKNESS 4 // thickness in px, should be even

// Public function declarations
void drawAllX(
    unsigned char originalImageArray[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS],
    int detectedCellsCoordinates[][2], // array of pairs (x, y)
    int detectedCellsNo
);

#endif