#ifndef STEP4CAPTURE_H
#define STEP4CAPTURE_H

#include "cbmp.h"

#define CAPTURE_SIZE 12
#define FRAME_CLEARANCE 1

#define HALF_CAPTURE_SIZE (CAPTURE_SIZE / 2)
#define HALF_TOTAL_CAPTURE_SIZE (HALF_CAPTURE_SIZE + FRAME_CLEARANCE)

struct CaptureResult {
    int chords[1000][2];
    int n;
};

void capture(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], struct CaptureResult* result);

#endif