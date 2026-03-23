#ifndef TESTGENERATETESTDATA_H
#define TESTGENERATETESTDATA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cbmp.h"
#include "Step1GrayScale.h"
#include "Step2BinaryThreshold.h"
#include "Step3Erode.h"
#include "Step4Capture.h"
#include "Step5MarkingCells.h"
#include "CellDetection.h"


extern bool g_generate_test_data;

struct ProcessingResults {
    unsigned char original[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
    unsigned char grayscale[BMP_WIDTH][BMP_HEIGTH];
    unsigned char binary[BMP_WIDTH][BMP_HEIGTH];
    unsigned char marked[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
    int chords[1000][2];
    int nChords;
};

static struct ProcessingResults testData;

void TESTS_save_test_data(const char* filename, struct ProcessingResults* results);
void TESTS_memcpy(void * __restrict__ __dst, const void * __restrict__ __src, size_t __n);

#endif