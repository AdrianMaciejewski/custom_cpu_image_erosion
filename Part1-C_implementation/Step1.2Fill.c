#include "Step1.2Fill.h"
#include <stdio.h>

char encased(int x, int y, unsigned char binaryImage[BMP_WIDTH][BMP_HEIGTH]) {
    for (int i=x-2; i<=x+2; i++) {
        if (binaryImage[i][y+2]==0) return 0;
        if (binaryImage[i][y-2]==0) return 0;
    }
    for (int i=y-2; i<=y+2; i++) {
        if (binaryImage[x+2][i]==0) return 0;
        if (binaryImage[x-2][i]==0) return 0;
    }
    return 1;
}

void fill(unsigned char binaryImage[BMP_WIDTH][BMP_HEIGTH]) {
    for (int x=2; x<BMP_WIDTH-2; x++) {
        for (int y=2; y<BMP_HEIGTH-2; y++) {
            if (encased(x, y, binaryImage)) {
                binaryImage[x][y] = 255;
                binaryImage[x+1][y-1] = 255;
                binaryImage[x+1][y] = 255;
                binaryImage[x+1][y+1] = 255;
                binaryImage[x][y-1] = 255;
                binaryImage[x][y+1] = 255;
                binaryImage[x-1][y+1] = 255;
                binaryImage[x-1][y] = 255;
                binaryImage[x-1][y-1] = 255;
            }
        }
    }
}

