#ifndef ASTROMETRYLIB_HPP
#define ASTROMETRYLIB_HPP

#define MAX_THREADS 12
#define BUF_SIZE 255

DWORD WINAPI SolverThread(LPVOID lpParam);

// Request implementation for unit
#define STB_IMAGE_IMPLEMENTATION

extern "C" {
#include "anidx.h"
#include "solver.h"
#include "onefield.h"
#include "sip-utils.h"
#include "mathutil.h"
#include "stb_image.h"
#include "sep.h"
}

typedef struct {
    sep_catalog* cat;
    int idx;
} sep_catalog_idx;

template <typename T>
void sep_resort_array(sep_catalog_idx* sorted, int n, T* arr)
{
    T* cpy = (T*)malloc(n * sizeof(T));
    if (cpy == NULL) exit(1);
    for (int i = 0; i < n; i += 1) {
        int dst = sorted[i].idx;
        cpy[i] = arr[dst];
    }
    memcpy(arr, cpy, n * sizeof(T));
    free(cpy);
}

#endif