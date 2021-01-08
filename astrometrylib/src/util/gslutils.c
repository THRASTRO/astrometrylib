/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#include <stdarg.h>

#include "compat.h"
#include "gslutils.h"

#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_errno.h>


int gslutils_solve_leastsquares_v(gsl_matrix* A, int NB, ...) {
    int i, res;
    gsl_vector**  B = malloc(NB * sizeof(gsl_vector*));
    // Whoa, three-star programming!
    gsl_vector*** X = malloc(NB * sizeof(gsl_vector**));
    gsl_vector*** R = malloc(NB * sizeof(gsl_vector**));

    gsl_vector** Xtmp = malloc(NB * sizeof(gsl_vector*));
    gsl_vector** Rtmp = malloc(NB * sizeof(gsl_vector*));

    va_list va;
    va_start(va, NB);
    for (i=0; i<NB; i++) {
        B[i] = va_arg(va, gsl_vector*);
        X[i] = va_arg(va, gsl_vector**);
        R[i] = va_arg(va, gsl_vector**);
    }
    va_end(va);

    res = gslutils_solve_leastsquares(A, B, Xtmp, Rtmp, NB);
    for (i=0; i<NB; i++) {
        if (X[i])
            *(X[i]) = Xtmp[i];
        else
            gsl_vector_free(Xtmp[i]);
        if (R[i])
            *(R[i]) = Rtmp[i];
        else
            gsl_vector_free(Rtmp[i]);
    }
    free(Xtmp);
    free(Rtmp);
    free(X);
    free(R);
    free(B);
    return res;
}

int gslutils_solve_leastsquares(gsl_matrix* A, gsl_vector** B,
                                gsl_vector** X, gsl_vector** resids,
                                int NB) {
    int i;
    gsl_vector *tau, *resid = NULL;
    int ret;
    int M, N;

    M = A->size1;
    N = A->size2;

    for (i=0; i<NB; i++) {
        assert(B[i]);
        assert(B[i]->size == M);
    }

    tau = gsl_vector_alloc(MIN(M, N));
    assert(tau);

    ret = gsl_linalg_QR_decomp(A, tau);
    assert(ret == 0);
    // A,tau now contains a packed version of Q,R.

    for (i=0; i<NB; i++) {
        if (!resid) {
            resid = gsl_vector_alloc(M);
            assert(resid);
        }
        X[i] = gsl_vector_alloc(N);
        assert(X[i]);
        ret = gsl_linalg_QR_lssolve(A, tau, B[i], X[i], resid);
        assert(ret == 0);
        if (resids) {
            resids[i] = resid;
            resid = NULL;
        }
    }

    gsl_vector_free(tau);
    if (resid)
        gsl_vector_free(resid);

    return 0;
}

