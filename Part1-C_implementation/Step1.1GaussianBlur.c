#include "Step1.1GaussianBlur.h"

// length = 2*r+1, where r ≈ 3*sigma
static int gaussianKernel1d(double sigma, double *k, int max_len) {
    int r = (int)ceil(3.0 * sigma);
    int len = 2*r + 1;
    if (len > max_len) {            
        len = max_len;
        r = (len - 1) / 2;
    }
    double s2 = 2.0 * sigma * sigma;
    double sum = 0.0;
    for (int i = -r; i <= r; ++i) {
        double v = exp(-(i*i) / s2);
        k[i + r] = v;
        sum += v;
    }
    for (int i = 0; i < len; ++i) k[i] /= sum;
    return r;
}

static void convolveRowsGauss(const unsigned char src[BMP_WIDTH][BMP_HEIGTH], float tmp[BMP_WIDTH][BMP_HEIGTH],
                                const double *k, int r) {
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGTH; ++y) {
            double acc = 0.0;
            for (int dy = -r; dy <= r; ++dy) {
                int yy = y + dy;
                if (yy < 0)     yy = 0;
                if (yy >= BMP_HEIGTH)    yy = BMP_HEIGTH - 1;
                acc += k[dy + r] * src[x][yy];
            }
            tmp[x][y] = (float)acc;
        }
    }
}

static void convolveColsGauss(const float tmp[BMP_WIDTH][BMP_HEIGTH], unsigned char dst[BMP_WIDTH][BMP_HEIGTH],
                                const double *k, int r) {
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGTH; ++y) {
            double acc = 0.0;
            for (int dx = -r; dx <= r; ++dx) {
                int xx = x + dx;
                if (xx < 0)     xx = 0;
                if (xx >= BMP_WIDTH)    xx = BMP_WIDTH - 1;
                acc += k[dx + r] * tmp[xx][y];
            }
            int v = (int)(acc + 0.5);
            if (v < 0)   v = 0;
            if (v > 255) v = 255;
            dst[x][y] = v;
        }
    }
}

//  Main function to perform Gaussian background subtraction

void bgSubtractGaussian(unsigned char input_array[BMP_WIDTH][BMP_HEIGTH], double sigma) {
    double kernel[1025];
    int r = gaussianKernel1d(sigma, kernel, 1025);

    static float tmp[BMP_WIDTH][BMP_HEIGTH];
    static unsigned char bg[BMP_WIDTH][BMP_HEIGTH];
    convolveRowsGauss(input_array, tmp, kernel, r);
    convolveColsGauss(tmp, bg, kernel, r);

    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGTH; ++y) {
            int v = (int)input_array[x][y] - (int)bg[x][y];
            if (v < 0)   v = 0;
            if (v > 255) v = 255;
            input_array[x][y] = (unsigned char)v;
        }
    }
}
