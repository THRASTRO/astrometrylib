/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "compat.h"
#include "mathutil.h"

void normalize(double* x, double* y, double* z) {
    double invl = 1.0 / sqrt((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
    *x *= invl;
    *y *= invl;
    *z *= invl;
}

void normalize_3(double* xyz) {
    double invlen = 1.0 / sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2]);
    xyz[0] *= invlen;
    xyz[1] *= invlen;
    xyz[2] *= invlen;
}

int imax(int a, int b) {
    return (a > b) ? a : b;
}

int imin(int a, int b) {
    return (a < b) ? a : b;
}

double distsq(const double* d1, const double* d2, int D) {
    double dist2;
    int i;
    dist2 = 0.0;
    for (i = 0; i < D; i++) {
        dist2 += square(d1[i] - d2[i]);
    }
    return dist2;
}

double square(double d) {
    return d * d;
}

// "borrowed" from <linux/bitops.h> from linux-2.4
static unsigned int my_hweight32(unsigned int w) {
    unsigned int res = (w & 0x55555555) + ((w >> 1) & 0x55555555);
    res = (res & 0x33333333) + ((res >> 2) & 0x33333333);
    res = (res & 0x0F0F0F0F) + ((res >> 4) & 0x0F0F0F0F);
    res = (res & 0x00FF00FF) + ((res >> 8) & 0x00FF00FF);
    return (res & 0x0000FFFF) + ((res >> 16) & 0x0000FFFF);
}

int invert_2by2_arr(const double* A, double* Ainv) {
    double det;
    double inv_det;
    det = A[0] * A[3] - A[1] * A[2];
    if (det == 0.0)
        return -1;
    inv_det = 1.0 / det;
    Ainv[0] =  A[3] * inv_det;
    Ainv[1] = -A[1] * inv_det;
    Ainv[2] = -A[2] * inv_det;
    Ainv[3] =  A[0] * inv_det;
    return 0;
}

int is_power_of_two(unsigned int x) {
    return (my_hweight32(x) == 1);
}

#define GAUSSIAN_SAMPLE_INVALID -1e300

double gaussian_sample(double mean, double stddev) {
    // from http://www.taygeta.com/random/gaussian.html
    // Algorithm by Dr. Everett (Skip) Carter, Jr.
    static double y2 = GAUSSIAN_SAMPLE_INVALID;
    double x1, x2, w, y1;

    // this algorithm generates random samples in pairs; the INVALID
    // jibba-jabba stores the second value until the next time the
    // function is called.

    if (y2 != GAUSSIAN_SAMPLE_INVALID) {
        y1 = y2;
        y2 = GAUSSIAN_SAMPLE_INVALID;
        return mean + y1 * stddev;
    }
    do {
        x1 = uniform_sample(-1, 1);
        x2 = uniform_sample(-1, 1);
        w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );

    w = sqrt( (-2.0 * log(w)) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    return mean + y1 * stddev;
}

double uniform_sample(double low, double high) {
    if (low == high) return low;
    return low + (high - low)*((double)rand() / (double)RAND_MAX);
}
