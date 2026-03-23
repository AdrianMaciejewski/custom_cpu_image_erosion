#include "Step2BinaryThreshold.h"
#include <stdio.h>

int otsu(unsigned char input_array[BMP_WIDTH][BMP_HEIGTH]){
    int hist[256] = {0};
    const int N = BMP_HEIGTH * BMP_WIDTH;

    // 1) Histogram
    for (int y = 0; y < BMP_HEIGTH; ++y) {
        const unsigned char *row = input_array[y];
        for (int x = 0; x < BMP_WIDTH; ++x) {
            hist[row[x]]++;
        }
    }

    // 2) Global intensity sum
    double sum = 0.0;
    for (int i = 0; i < 256; ++i) {
        sum += (double)i * (double)hist[i];
    }

    // 3) Sweep thresholds to maximize between-class variance
    double sumB = 0.0;   // cumulative intensity sum (background)
    int    wB   = 0;     // cumulative count (background)
    int    bestT = 0;
    double bestVar = -1.0;

    for (int t = 0; t < 256; ++t) {
        wB += hist[t];
        if (wB == 0) continue;

        int wF = N - wB;
        if (wF == 0) break;

        sumB += (double)t * (double)hist[t];
        double mB = sumB / wB;
        double mF = (sum - sumB) / wF;

        double diff = mB - mF;
        double varBetween = (double)wB * (double)wF * diff * diff;

        if (varBetween > bestVar) {
            bestVar = varBetween;
            bestT   = t;
        }
    }
    return bestT;
}

void toBinaryScale(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], int threshold) {
    // threshold = otsu(input_image);
    for (int x=0; x < BMP_WIDTH; x++) {
        for (int y=0; y < BMP_HEIGTH; y++) {
            input_image[x][y] = input_image[x][y] < threshold ? 0 : 255;
        }
    }
}