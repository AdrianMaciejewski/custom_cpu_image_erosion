#include "Step1GrayScale.h"

void toGrayScale(unsigned char input_array[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_array[BMP_WIDTH][BMP_HEIGTH]){
        for (int i = 0; i < BMP_WIDTH; i++) {
            for (int j = 0; j < BMP_HEIGTH; j++) {
                int sum = 0;
                for (int c = 0; c < BMP_CHANNELS; c++) {
                    sum += input_array[i][j][c];
                }
                output_array[i][j] = sum / BMP_CHANNELS;
            }
        }
        
}