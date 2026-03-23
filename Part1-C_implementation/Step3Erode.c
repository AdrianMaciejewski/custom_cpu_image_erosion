#include "Step3Erode.h"
#include <stdio.h>

// --- shape ---
#define erosionShapeSize 3
#define erosionShapeDiameter (erosionShapeSize/2) // 1 for size 3, 2 for size 5, etc.

// Define the erosion shape
static const char erosionShape[erosionShapeSize][erosionShapeSize] = {
  {0,1,0},
  {1,1,1},
  {0,1,0}
};

static char areErosionShapeOffsetsInitialized = 0;
static int erosionShapeOffsets[erosionShapeSize*erosionShapeSize][2]; // for efficiency, use indexes of 1s in erosionShape
static int erosionShapeOffsetsCount = 0;

// convenience function to get the offsets of the erosion shape
int getErosionShapeIndexOffsets() {
    int count = 0;
    for (int xOffset = -erosionShapeDiameter; xOffset <= erosionShapeDiameter; xOffset++) {
        for (int yOffset = -erosionShapeDiameter; yOffset <= erosionShapeDiameter; yOffset++) {
            if (erosionShape[xOffset + erosionShapeDiameter][yOffset + erosionShapeDiameter] == 1) {
                erosionShapeOffsets[count][0] = xOffset;
                erosionShapeOffsets[count][1] = yOffset;
                count++;
            }
        }
    }
    return count;
}

void initializeErosionShape() {
    if (areErosionShapeOffsetsInitialized) return;
    erosionShapeOffsetsCount = getErosionShapeIndexOffsets();
    areErosionShapeOffsetsInitialized = 1;
}

// --- erosion ---
char isOutOfBounds(int x, int y) {
    return x<0 || x>=BMP_WIDTH || y<0 || y>=BMP_HEIGTH;
}

char shouldBeEroded(int x, int y, unsigned char binaryImage[BMP_WIDTH][BMP_HEIGTH]) {
    for (int i=0; i<erosionShapeOffsetsCount; i++) {
        int* offset = erosionShapeOffsets[i];
        int neighborX = x + offset[0];
        int neighborY = y + offset[1];
        if (!isOutOfBounds(neighborX, neighborY) && !binaryImage[neighborX][neighborY]) {
            return 1;
        }
    }
    return 0;
}

char erode(unsigned char binaryImage[BMP_WIDTH][BMP_HEIGTH], unsigned char outputImage[BMP_WIDTH][BMP_HEIGTH]) {
    initializeErosionShape(); // initialize the offsets if not done yet

    char isFullyEroded = 1;
    for (int x=0; x<BMP_WIDTH; x++) {
        for (int y=0; y<BMP_HEIGTH; y++) {
            if (binaryImage[x][y]==0)
            {
                outputImage[x][y] = 0;
                continue;
            }

            outputImage[x][y] = !shouldBeEroded(x, y, binaryImage); // 1 if should not be eroded, 0 if should be eroded
            isFullyEroded = 0;
        }
    }
    return isFullyEroded;
}