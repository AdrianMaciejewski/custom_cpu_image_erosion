#include "TestDataGeneration.h"


bool g_generate_test_data = false; // default off

void TESTS_memcpy(void * __restrict__ __dst, const void * __restrict__ __src, size_t __n) {
    if (!g_generate_test_data) {
        return;
    }
    memcpy(__dst, __src, __n);
}

void TESTS_save_test_data(const char* filename, struct ProcessingResults* results) {
    if (!g_generate_test_data) {
        return;
    }

    FILE* f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Could not open %s for writing\n", filename);
        return;
    }

    // Include header file import
    fprintf(f, "#include \"TestData.h\"\n\n");

    // Original
    fprintf(f, "unsigned char original[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS] = {\n");
    for (int i = 0; i < BMP_WIDTH; i++) {
        fprintf(f, "    {\n");
        for (int j = 0; j < BMP_HEIGTH; j++) {
            fprintf(f, "        {%u, %u, %u}", 
                results->original[i][j][0], 
                results->original[i][j][1], 
                results->original[i][j][2]);
            if (j < BMP_HEIGTH - 1) fprintf(f, ", ");
        }
        fprintf(f, "    }%s\n", (i < BMP_WIDTH - 1) ? "," : "");
    }
    fprintf(f, "};\n\n");

    // Grayscale
    fprintf(f, "unsigned char grayscale[BMP_WIDTH][BMP_HEIGTH] = {\n");
    for (int i = 0; i < BMP_WIDTH; i++) {
        fprintf(f, "    {");
        for (int j = 0; j < BMP_HEIGTH; j++) {
            fprintf(f, "%u", results->grayscale[i][j]);
            if (j < BMP_HEIGTH - 1) fprintf(f, ", ");
        }
        fprintf(f, "}%s\n", (i < BMP_WIDTH - 1) ? "," : "");
    }
    fprintf(f, "};\n\n");

    // Binary
    fprintf(f, "unsigned char binary[BMP_WIDTH][BMP_HEIGTH] = {\n");
    for (int i = 0; i < BMP_WIDTH; i++) {
        fprintf(f, "    {");
        for (int j = 0; j < BMP_HEIGTH; j++) {
            fprintf(f, "%u", results->binary[i][j]);
            if (j < BMP_HEIGTH - 1) fprintf(f, ", ");
        }
        fprintf(f, "}%s\n", (i < BMP_WIDTH - 1) ? "," : "");
    }
    fprintf(f, "};\n\n");

    // Marked image
    fprintf(f, "unsigned char marked[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS] = {\n");
    for (int i = 0; i < BMP_WIDTH; i++) {
        fprintf(f, "    {\n");
        for (int j = 0; j < BMP_HEIGTH; j++) {
            fprintf(f, "        {%u, %u, %u}", 
                results->marked[i][j][0], 
                results->marked[i][j][1], 
                results->marked[i][j][2]);
            if (j < BMP_HEIGTH - 1) fprintf(f, ", ");
        }
        fprintf(f, "    }%s\n", (i < BMP_WIDTH - 1) ? "," : "");
    }
    fprintf(f, "};\n\n");

    // Chords
    fprintf(f, "int chords[%d][2] = {\n", results->nChords);
    for (int i = 0; i < results->nChords; i++) {
        fprintf(f, "    {%d, %d}", results->chords[i][0], results->chords[i][1]);
        if (i < results->nChords - 1) fprintf(f, ",\n");
        else fprintf(f, "\n");
    }
    fprintf(f, "};\n");
    fprintf(f, "int nChords = %d;\n", results->nChords);

    fclose(f);

    printf("[TESTS] Test data saved to %s\n", filename);
}