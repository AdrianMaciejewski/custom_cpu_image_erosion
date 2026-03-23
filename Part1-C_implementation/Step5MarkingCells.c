#include "Step5MarkingCells.h"


static void setPixelColor(unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int x, int y, uint32_t color
) {
    if (x >= 0 && x < BMP_WIDTH && y >= 0 && y < BMP_HEIGTH) {
        image[x][y][0] = (unsigned char)((color >> 16) & 0xFF);
        image[x][y][1] = (unsigned char)((color >>  8) & 0xFF);
        image[x][y][2] = (unsigned char)((color      ) & 0xFF);
    }
}

static void drawX(
    unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int xSize, int xCenter, int yCenter
) {
    const int xArmSize = (xSize - 1) / 2;

    for (int offset = -xArmSize; offset <= xArmSize; offset++) {
        int x = xCenter + offset;

        int yUp = yCenter + offset;
        int yDown = yCenter - offset;
        for (int t = -X_THICKNESS/2; t <= X_THICKNESS/2; t++) {
            setPixelColor(image, x, yUp + t, X_COLOR);
            setPixelColor(image, x, yDown + t, X_COLOR);
            setPixelColor(image, x + t, yUp, X_COLOR);
            setPixelColor(image, x + t, yDown, X_COLOR);
        }
    }
}

void drawAllX(
    unsigned char originalImageArray[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS],
    int detectedCellsCoordinates[][2],
    int detectedCellsNo
) {
    for (int i = 0; i < detectedCellsNo; i++) {
        const int xCenter = detectedCellsCoordinates[i][0];
        const int yCenter = detectedCellsCoordinates[i][1];
        drawX(originalImageArray, X_SIZE, xCenter, yCenter);
    }
}