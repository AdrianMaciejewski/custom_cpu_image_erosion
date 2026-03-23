#include "CellDetection.h"

struct CaptureResult erodeAndCaptureAll(unsigned char inputImage[BMP_WIDTH][BMP_HEIGTH])
{
  struct CaptureResult result = { .n = 0 };
  
  unsigned char (*inputImagePtr)[BMP_HEIGTH] = inputImage;
  
  unsigned char outputImage[BMP_WIDTH][BMP_HEIGTH];
  unsigned char (*outputImagePtr)[BMP_HEIGTH] = outputImage;

  int i=0;
  while (!erode(inputImagePtr, outputImagePtr)) {
      if (g_debug)
      {
        char filename[50];
        sprintf(filename, ".\\output_images\\OUTeroded%d.bmp", i);
        DEBUG_write_binary_bitmap(outputImagePtr, filename);
      }
      
      if (i>2)
      {
        // get rid off all the grain from gaussian noise and binary thresholding before capturing
        capture(outputImagePtr, &result);
        DEBUG_PRINT("After erosion step %d, captured %d chords\n", i, result.n);
      }
      
      // Swap temp1 and temp2 pointers
      unsigned char (*tmp)[BMP_HEIGTH] = outputImagePtr;
      outputImagePtr = inputImagePtr;
      inputImagePtr = tmp;

      // Increment step counter
      i++;
  }
  DEBUG_PRINT("Captured %d chords:\n", result.n);

  return result;
}

int detectCells(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], double sigma, int threshold){
  TESTS_memcpy(testData.original, input_image, BMP_WIDTH * BMP_HEIGTH * BMP_CHANNELS * sizeof(unsigned char));
  unsigned char processedImage[BMP_WIDTH][BMP_HEIGTH];

  // Step 1: Grayscale
  toGrayScale(input_image, processedImage);
  TESTS_memcpy(testData.grayscale, processedImage, sizeof(processedImage));
  DEBUG_write_grayScale_bitmap(processedImage, ".\\output_images\\OUTgrayscale.bmp");

  // Step 1.1: Filter noice - Gaussian Background Subtraction
  bgSubtractGaussian(processedImage, sigma);
  DEBUG_write_grayScale_bitmap(processedImage, ".\\output_images\\OUTbgSubtracted.bmp");

  // Step 1.2: Fill
  fill(processedImage);
  DEBUG_write_grayScale_bitmap(processedImage, ".\\output_images\\OUTfilled.bmp");

  // Step 2: Binary Threshold
  toBinaryScale(processedImage, threshold);
  TESTS_memcpy(testData.binary, processedImage, sizeof(processedImage));
  DEBUG_write_binary_bitmap(processedImage, ".\\output_images\\OutBinary.bmp");

  // Step 3 & 4: Erode and Capture
  struct CaptureResult result = erodeAndCaptureAll(processedImage);
  TESTS_memcpy(testData.chords, result.chords, sizeof(result.chords));
  TESTS_memcpy(&(testData.nChords), &(result.n), sizeof(result.n));

  // Step 5: Marking Cells with X
  drawAllX(input_image, result.chords, result.n);
  TESTS_memcpy(testData.marked, input_image, BMP_WIDTH * BMP_HEIGTH * BMP_CHANNELS * sizeof(unsigned char));
  TESTS_save_test_data(".\\TestData.c", &testData);

  return result.n;
}