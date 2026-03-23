#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdbool.h>
#include "cbmp.h"

extern bool g_debug;

#define DEBUG_PRINT(fmt, ...) \
    do { if (g_debug) fprintf(stderr, "[DEBUG] " fmt, ##__VA_ARGS__); } while (0)

#endif
