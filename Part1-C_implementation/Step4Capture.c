#include "Step4Capture.h"
#include <stdio.h>
#include "Step3Erode.h"


char isPointOnImageBorder(int x, int y) {
    return (x == 0 || x == BMP_WIDTH-1 || y == 0 || y == BMP_HEIGTH-1);
}

char isExclusionFrameClear(int x, int y, unsigned char arr[BMP_WIDTH][BMP_HEIGTH]) {
    const int sideCenters[4] = {
        x + HALF_TOTAL_CAPTURE_SIZE, // right
        x - HALF_TOTAL_CAPTURE_SIZE, // left
        y + HALF_TOTAL_CAPTURE_SIZE, // top
        y - HALF_TOTAL_CAPTURE_SIZE  // bottom
    };
    
    for (int s = 0; s < 4; s++) {
        char isHorizontalSide = (s < 2);
        if(isPointOnImageBorder(isHorizontalSide ? sideCenters[s] : x, !isHorizontalSide ? sideCenters[s] : y)) {
            continue; // if we are on the border, we assume that the frame is clear and continue checking other sides
        }
        
        for (char offset=-HALF_TOTAL_CAPTURE_SIZE; offset<=HALF_TOTAL_CAPTURE_SIZE; offset++)
        {
            const int xFrame = isHorizontalSide ? sideCenters[s] : x+offset; // if right or left side, then x is constant
            const int yFrame = !isHorizontalSide ? sideCenters[s] : y+offset; // if top or bottom side, then y is constant
            if (arr[xFrame][yFrame]) {
                return 0;
            }
        }
    }

    return 1;
}

void capture(unsigned char binaryImage[BMP_WIDTH][BMP_HEIGTH], struct CaptureResult* result) {
    for (int y=HALF_TOTAL_CAPTURE_SIZE; y<BMP_HEIGTH-HALF_TOTAL_CAPTURE_SIZE; y++) {
        for (int x=HALF_TOTAL_CAPTURE_SIZE; x<BMP_WIDTH-HALF_TOTAL_CAPTURE_SIZE; x++) {
            // skip if cell is on the border
            if (!isExclusionFrameClear(x, y, binaryImage))
            {
                continue;
            }

            // check for cell presence in the capture area
            char isCellDetected=0;
            for (int offsetX=-HALF_CAPTURE_SIZE; offsetX<=HALF_CAPTURE_SIZE; offsetX++) {
                for (int offsetY=-HALF_CAPTURE_SIZE; offsetY<=HALF_CAPTURE_SIZE; offsetY++) {
                    if (binaryImage[x+offsetX][y+offsetY]) {
                        isCellDetected = 1;
                        binaryImage[x+offsetX][y+offsetY] = 0;
                    }
                }
            }

            // add to result if a cell was detected
            if (isCellDetected) {
                // printf("Cell detected at (%d, %d)\n", x, y);
                result->chords[result->n][0] = x;
                result->chords[result->n][1] = y;
                result->n++;
            }
            
            if (x+CAPTURE_SIZE<BMP_WIDTH-HALF_TOTAL_CAPTURE_SIZE)
            {
                x += CAPTURE_SIZE; // Skip ahead since we've cleared this area - no need to check overlapping positions
                x -= 1; // adjust for the upcoming y++ in the for loop
            }
        }
    }
}