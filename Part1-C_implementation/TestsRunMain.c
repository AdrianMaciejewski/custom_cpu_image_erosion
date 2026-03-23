#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cbmp.h"
#include "Step1GrayScale.h"
#include "Step2BinaryThreshold.h"
#include "Step3Erode.h"
#include "Step4Capture.h"
#include "Step5MarkingCells.h"
#include "CellDetection.h"
#include "TestData.h"


typedef void (*func_t)(void);
double measure_execution_time(func_t func) {
    clock_t start = clock();
    func();
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

/* Global file pointer for logging */
static FILE *log_file = NULL;

/* Custom printf that writes to both console and file */
#define printf(...) do { \
    fprintf(stdout, __VA_ARGS__); \
    if(log_file) fprintf(log_file, __VA_ARGS__); \
} while(0)

/* --------------------- tiny assertion helpers --------------------- */

#define FAIL(...) do{ \
    fprintf(stderr, __VA_ARGS__); \
    fputc('\n', stderr); \
    if(log_file) { fprintf(log_file, __VA_ARGS__); fputc('\n', log_file); } \
}while(0)
#define ASSERT_TRUE(cond, ...) do{ if(!(cond)){ FAIL(__VA_ARGS__); return 0; } }while(0)
#define ASSERT_EQ_INT(a,b,...) do{ long long _A=(a), _B=(b); if(_A!=_B){ \
  FAIL("Expected %s == %s, got %lld vs %lld. %s", #a, #b, _A, _B, "" __VA_ARGS__); return 0; } }while(0)

/* Compare 2D u8 arrays stored as [BMP_WIDTH][BMP_HEIGTH] (column-major like your code). */
static int diff_u8_2d(const unsigned char *A, const unsigned char *B, int max_report){
    int diffs=0;
    for(int x=0;x<BMP_WIDTH;++x){
        const unsigned char* rowA = A + x*BMP_HEIGTH;
        const unsigned char* rowB = B + x*BMP_HEIGTH;
        for(int y=0;y<BMP_HEIGTH;++y){
            if(rowA[y]!=rowB[y]){
                if(diffs<max_report){
                    fprintf(stderr,"  diff at (%d,%d): %u vs %u\n",x,y,(unsigned)rowA[y],(unsigned)rowB[y]);
                    if(log_file) fprintf(log_file,"  diff at (%d,%d): %u vs %u\n",x,y,(unsigned)rowA[y],(unsigned)rowB[y]);
                }
                ++diffs;
            }
        }
    }
    return diffs;
}

/* Compare 3D u8 arrays stored as [BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]. */
static int diff_u8_3d(const unsigned char *A, const unsigned char *B, int max_report){
    int diffs=0;
    for(int x=0;x<BMP_WIDTH;++x){
        for(int y=0;y<BMP_HEIGTH;++y){
            const unsigned char* pA = A + (x*BMP_HEIGTH + y)*BMP_CHANNELS;
            const unsigned char* pB = B + (x*BMP_HEIGTH + y)*BMP_CHANNELS;
            for(int c=0;c<BMP_CHANNELS;++c){
                if(pA[c]!=pB[c]){
                    if(diffs<max_report){
                        fprintf(stderr,"  diff at (%d,%d,c=%d): %u vs %u\n",
                                x,y,c,(unsigned)pA[c],(unsigned)pB[c]);
                        if(log_file) fprintf(log_file,"  diff at (%d,%d,c=%d): %u vs %u\n",
                                x,y,c,(unsigned)pA[c],(unsigned)pB[c]);
                    }
                    ++diffs;
                }
            }
        }
    }
    return diffs;
}

static int chords_in_bounds(const int (*chords)[2], int n){
    for(int i=0;i<n;++i){
        int x=chords[i][0], y=chords[i][1];
        if(x<0||x>=BMP_WIDTH||y<0||y>=BMP_HEIGTH) return 0;
    }
    return 1;
}

/* --------------------- Test Cases (isolated per stage) --------------------- */
/* Each stage has:
   - a properties/invariants test
   - a separate golden/expected test
*/

/* ---- Step 1: toGrayScale ---- */

static int test_grayscale_properties(void){
    static unsigned char input[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
    static unsigned char out  [BMP_WIDTH][BMP_HEIGTH];

    /* Use copy of original from TestData as input. */
    memcpy(input, original, sizeof(input));
    toGrayScale(input, out);

    /* Invariants: values in [0,255], image non-constant ideally */
    int minv=255, maxv=0;
    for(int x=0;x<BMP_WIDTH;++x){
        for(int y=0;y<BMP_HEIGTH;++y){
            unsigned v=out[x][y];
            if((int)v<minv) minv=v;
            if((int)v>maxv) maxv=v;
        }
    }
    ASSERT_TRUE(minv>=0 && maxv<=255, "Grayscale range out of bounds");
    ASSERT_TRUE(maxv>minv, "Grayscale looks constant (suspicious)");
    return 1;
}

static int test_grayscale_expected(void){
    static unsigned char input[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
    static unsigned char out  [BMP_WIDTH][BMP_HEIGTH];

    memcpy(input, original, sizeof(input));
    toGrayScale(input, out);

    int diffs = diff_u8_2d((const unsigned char*)out,
                           (const unsigned char*)grayscale, 10);
    ASSERT_EQ_INT(diffs, 0, "Grayscale does not match expectedGray");
    return 1;
}

/* ---- Step 2: toBinaryScale ---- */

static int test_binary_properties(void){
    static unsigned char inBin[BMP_WIDTH][BMP_HEIGTH];
    static unsigned char out  [BMP_WIDTH][BMP_HEIGTH];

    /* Input is the EXPECTED grayscale, not computed here. */
    memcpy(inBin, grayscale, sizeof(inBin));

    /* Work in-place per your API; copy into out first to avoid mutating golden. */
    memcpy(out, inBin, sizeof(out));
    toBinaryScale(out, THRESHOLD);

    int zeros=0, twf=0, other=0;
    for(int x=0;x<BMP_WIDTH;++x){
        for(int y=0;y<BMP_HEIGTH;++y){
            unsigned v=out[x][y];
            if(v==0) zeros++;
            else if(v==255) twf++;
            else other++;
        }
    }
    ASSERT_EQ_INT(other, 0, "Binary image has values not in {0,255}");
    ASSERT_TRUE(zeros>0, "Binary image is all white");
    ASSERT_TRUE(twf>0,   "Binary image is all black");
    return 1;
}

static int test_binary_expected(void){
    static unsigned char inBin[BMP_WIDTH][BMP_HEIGTH];
    static unsigned char out  [BMP_WIDTH][BMP_HEIGTH];

    memcpy(inBin, grayscale, sizeof(inBin));
    memcpy(out, inBin, sizeof(out));
    toBinaryScale(out, THRESHOLD);

    int diffs = diff_u8_2d((const unsigned char*)out,
                           (const unsigned char*)binary, 10);
    ASSERT_EQ_INT(diffs, 0, "Binary does not match expectedBinary");
    return 1;
}

/* ---- Step 3+4: erodeAndCaptureAll ---- */

static int test_erode_capture_properties(void){
    static unsigned char inB[BMP_WIDTH][BMP_HEIGTH];
    memcpy(inB, binary, sizeof(inB));

    struct CaptureResult res = erodeAndCaptureAll(inB);

    ASSERT_TRUE(res.n >= 0 && res.n <= 1000, "Chord count out of range");
    ASSERT_TRUE(chords_in_bounds(res.chords, res.n), "Some chords out of bounds");
    return 1;
}



/* ---- Step 3: erode cycles measurement ---- */
// Function to read the Time Stamp Counter
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

static void test_erode_cycles(void){
    unsigned char outputImage[BMP_WIDTH][BMP_HEIGTH];
    static unsigned char inB[BMP_WIDTH][BMP_HEIGTH];
    memcpy(inB, binary, sizeof(inB));

    uint64_t start, end;

    // warmup cache
    start = rdtsc();
    erode(inB, outputImage);
    end = rdtsc();
    
    
    // Multiple measurements for better accuracy
    printf("\nMultiple measurements:\n");
    uint64_t total_cycles = 0;
    int runs = 1000;
    for (int i = 0; i < runs; i++) {
        start = rdtsc();
        erode(inB, outputImage);
        end = rdtsc();
        uint64_t cycles = end - start;
        total_cycles += cycles;
        printf("Run %d: %llu cycles\n", i + 1, (unsigned long long)cycles);
    }
    printf("Average: %llu cycles\n", (unsigned long long)(total_cycles / runs));
}

static int test_erode_capture_expected(void){
    static unsigned char inB[BMP_WIDTH][BMP_HEIGTH];
    memcpy(inB, binary, sizeof(inB));

    struct CaptureResult res = erodeAndCaptureAll(inB);

    ASSERT_EQ_INT(res.n, nChords, "Chord count differs from expectedN");

    int mismatch = 0;
    for(int i=0;i<res.n && i<nChords; ++i){
        if(res.chords[i][0]!=chords[i][0] ||
           res.chords[i][1]!=chords[i][1]){
            if(mismatch<10){
                fprintf(stderr,"  chord[%d]: got (%d,%d), expected (%d,%d)\n",
                        i,res.chords[i][0],res.chords[i][1],
                        chords[i][0],chords[i][1]);
                if(log_file) fprintf(log_file,"  chord[%d]: got (%d,%d), expected (%d,%d)\n",
                        i,res.chords[i][0],res.chords[i][1],
                        chords[i][0],chords[i][1]);
            }
            ++mismatch;
        }
    }
    ASSERT_EQ_INT(mismatch, 0, "Chord coordinates differ from expected");
    return 1;
}

/* ---- Step 5: drawAllX ---- */

static int test_draw_marks_properties(void){
    static unsigned char img[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];

    /* Start from original input copy and draw expected chords onto it. */
    memcpy(img, original, sizeof(img));
    drawAllX(img, chords, nChords);

    /* It should modify some pixels */
    int changed = memcmp(img, original, sizeof(img)) != 0;
    ASSERT_TRUE(changed, "drawAllX did not modify any pixel");
    return 1;
}

static int test_draw_marks_expected(void){
    static unsigned char img[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];

    memcpy(img, original, sizeof(img));
    drawAllX(img, chords, nChords);

    int diffs = diff_u8_3d((const unsigned char*)img,
                           (const unsigned char*)marked, 10);
    ASSERT_EQ_INT(diffs, 0, "Marked image differs from expectedMarked");
    return 1;
}

/* ---- Benchmarks ---- */
static void benchToGrayScale(void){
    static unsigned char input[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
    static unsigned char out  [BMP_WIDTH][BMP_HEIGTH];
    memcpy(input, original, sizeof(original));

    toGrayScale(input, out);
}
static void benchToBinaryScale(void) {
    static unsigned char img[BMP_WIDTH][BMP_HEIGTH];
    memcpy(img, grayscale, sizeof(img));

    toBinaryScale(img, THRESHOLD);
}

void benchErodeAndCapture()
{
    printf("\t\t3&4.erodeAndCaptureUntillDone steps:\n");
    static unsigned char inputImage[BMP_WIDTH][BMP_HEIGTH];
    memcpy(inputImage, binary, sizeof(binary));

    unsigned char (*inputImagePtr)[BMP_HEIGTH] = inputImage;

    unsigned char outputImage[BMP_WIDTH][BMP_HEIGTH];
    unsigned char (*outputImagePtr)[BMP_HEIGTH] = outputImage;

    double totalTime=0.0;
    struct CaptureResult result = { .n = 0 };
    char isFullyEroded = 0;
    int i=0;
    while (!isFullyEroded) {
        clock_t start1 = clock();
        isFullyEroded = erode(inputImagePtr, outputImagePtr);
        clock_t end1 = clock();
        double erosionTime = (double)(end1 - start1) / CLOCKS_PER_SEC;
        
        clock_t start2 = clock();
        capture(outputImagePtr, &result);
        clock_t end2 = clock();
        double captureTime = (double)(end2 - start2) / CLOCKS_PER_SEC;
        
        printf("\t\t\t%d. Erosion: %.6fs, Capture %.6fs\n", i+1, erosionTime, captureTime);
        
        // Swap temp1 and temp2 pointers
        unsigned char (*tmp)[BMP_HEIGTH] = outputImagePtr;
        outputImagePtr = inputImagePtr;
        inputImagePtr = tmp;

        totalTime += erosionTime + captureTime;
        i++;
    }

    printf("\t\t\tTotal: %.6f s\n", totalTime);
}
static void benchDrawAllX(void) {
    static unsigned char img[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
    memcpy(img, original, sizeof(img));

    drawAllX(img, chords, nChords);
}
static void benchWholeProgram(void) {
    static unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
    memcpy(input_image, original, sizeof(input_image));

    detectCells(input_image, SIGMA, THRESHOLD);
}

/* Performance test for cell detection */
static void test_cell_detection_performance() {
    char* imagePath[12] = {
        "./samples/easy/1EASY.bmp",
        "./samples/easy/5EASY.bmp",
        "./samples/easy/10EASY.bmp",
        "./samples/medium/1MEDIUM.bmp",
        "./samples/medium/5MEDIUM.bmp",
        "./samples/medium/10MEDIUM.bmp",
        "./samples/hard/1HARD.bmp",
        "./samples/hard/5HARD.bmp",
        "./samples/hard/10HARD.bmp",
        "./samples/impossible/1IMPOSSIBLE.bmp",
        "./samples/impossible/3IMPOSSIBLE.bmp",
        "./samples/impossible/5IMPOSSIBLE.bmp",
    };
    int totalSum = 0;
    int difficultySum = 0;
    for (int i = 0; i < 12; i++) {
        static unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
        read_bitmap(imagePath[i], input_image);
        int resultCount = detectCells(input_image, SIGMA, THRESHOLD);
        difficultySum += resultCount;
        totalSum += resultCount;
        const char *filename = strrchr(imagePath[i], '/');
        filename = filename ? filename + 1 : imagePath[i];

        printf("\t\tDetected %d cells for '%s'\n", resultCount, filename);
        if (i % 3 == 2)
        {
            printf("\t\tDetected on average: %d cells\n\n", difficultySum / 3);
            difficultySum = 0;
        }
    }
    printf("\t\tAverage cells no. Detected: %d cells\n\n", totalSum / 12);
}

/* --------------------- simple runner --------------------- */
static int run_test(int (*fn)(void), const char* name){
    int ok = fn();
    printf("[%s] %s\n", ok ? "PASS":"FAIL", name);
    return ok;
}

int main(void){
    // Open log file for writing
    log_file = fopen("test_results.txt", "w");
    if (log_file == NULL) {
        fprintf(stderr, "Warning: Could not open test_results.txt for writing\n");
    }
    
    int passed=0, total=0;

#define RUN(T) do{ ++total; passed += run_test(T, #T); }while(0)
    test_erode_cycles();

    RUN(test_grayscale_properties);
    RUN(test_grayscale_expected);

    RUN(test_binary_properties);
    RUN(test_binary_expected);

    RUN(test_erode_capture_properties);
    RUN(test_erode_capture_expected);

    RUN(test_draw_marks_properties);
    RUN(test_draw_marks_expected);

    // Benchmark summary
    printf("\nTime Benchmarks:\n");
    printf("\tPer step:\n");
    printf("\t\t1.toGrayScale:       %.6f s\n", measure_execution_time(benchToGrayScale));
    printf("\t\t2.toBinaryScale:     %.6f s\n", measure_execution_time(benchToBinaryScale));
    benchErodeAndCapture();
    printf("\t\t5.drawAllX:          %.6f s\n", measure_execution_time(benchDrawAllX));
    printf("\tWhole program:         %.6f s\n", measure_execution_time(benchWholeProgram));

    // Performance test
    printf("\nPerformance test:\n");
    test_cell_detection_performance();

#undef RUN

    printf("\nSummary: %d/%d tests passed\n", passed, total);
    
    // Close the log file
    if (log_file) {
        fclose(log_file);
    }
    
    return (passed==total) ? 0 : 1;
}
