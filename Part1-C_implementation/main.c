#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"
#include "Step1GrayScale.h"
#include "Step2BinaryThreshold.h"
#include "Step3Erode.h"
#include "Step4Capture.h"
#include "Step5MarkingCells.h"
#include "CellDetection.h"
#include "Debug.h"


//Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];

//Main function
int main(int argc, char** argv)
{
    double sigma = SIGMA;
    int threshold = THRESHOLD;
    if (argc <3 || argc > 9)
    {
        fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
        printf("Optional flags:\n");
        printf(" --debug : Enable debug output e.g. printing and saving intermediary result images\n");
        printf(" --generate_test_data : Generate test data file TestData.c\n");
        printf(" --sigma <value> : Set the sigma value for Gaussian background subtraction\n");
        printf(" --threshold <value> : Set the threshold value for binary thresholding (0-255)\n");
        exit(1);
    }
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) {
            g_debug = true;
        }
        if (strcmp(argv[i], "--generate_test_data") == 0) {
            g_generate_test_data = true;
        }
            if (strcmp(argv[i], "--sigma") == 0 && i + 1 < argc) {
                double input_sigma = atof(argv[i + 1]);
                if (input_sigma > 0) {
                    sigma = input_sigma;
                } else {
                    fprintf(stderr, "Invalid sigma value. Using default sigma = %f.\n", sigma);
                }
                i++; // Skip the next argument as it's the sigma value
            }
            if (strcmp(argv[i], "--threshold") == 0 && i + 1 < argc) {
                int input_threshold = atoi(argv[i + 1]);
                if (input_threshold >= 0 && input_threshold <= 255) {
                    threshold = input_threshold;
                } else {
                    fprintf(stderr, "Invalid threshold value. Using default threshold = %d.\n", threshold);
                }
                i++; // Skip the next argument as it's the threshold value
            }
    }

    read_bitmap(argv[1], input_image);
    detectCells(input_image, sigma, threshold);
    write_bitmap(input_image, argv[2]);

    printf("Done!\n");
    return 0;
}
