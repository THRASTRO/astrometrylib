/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#include "compat.h"
#include "kdtree.h"
#include "starkd.h"
#include "starutil.h"

static startree_t* startree_alloc() {
    startree_t* s = calloc(1, sizeof(startree_t));
    if (!s) {
        fprintf(stderr, "Failed to allocate a star kdtree struct.\n");
        return NULL;
    }
    return s;
}

void startree_search_for(const startree_t* s, const double* xyzcenter, double radius2,
                         double** xyzresults, double** radecresults,
                         int** starinds, int* nresults) {
    kdtree_qres_t* res = NULL;
    int opts;
    double* xyz;
    int i, N;

    opts = KD_OPTIONS_SMALL_RADIUS;
    if (xyzresults || radecresults)
        opts |= KD_OPTIONS_RETURN_POINTS;

    res = kdtree_rangesearch_options(s->tree, xyzcenter, radius2, opts);
	
    if (!res || !res->nres) {
        if (xyzresults)
            *xyzresults = NULL;
        if (radecresults)
            *radecresults = NULL;
        if (starinds)
            *starinds = NULL;
        *nresults = 0;
        if (res)
            kdtree_free_query(res);
        return;
    }

    xyz = res->results.d;
    N = res->nres;
    *nresults = N;

    if (radecresults) {
        *radecresults = malloc(N * 2 * sizeof(double));
        for (i=0; i<N; i++)
            xyzarr2radecdegarr(xyz + i*3, (*radecresults) + i*2);
    }
    if (xyzresults) {
        // Steal the results array.
        *xyzresults = xyz;
        res->results.d = NULL;
    }
    if (starinds) {
        *starinds = malloc(res->nres * sizeof(int));
        for (i=0; i<N; i++)
            (*starinds)[i] = res->inds[i];
    }
    kdtree_free_query(res);
}


void startree_search(const startree_t* s, const double* xyzcenter, double radius2,
                     double** xyzresults, double** radecresults, int* nresults) {
    startree_search_for(s, xyzcenter, radius2, xyzresults, radecresults, NULL, nresults);
}

int startree_close(startree_t* s) {
    if (!s) return 0;
    if (s->inverse_perm)
        free(s->inverse_perm);
    free(s);
    return 0;
}

static int Ndata(const startree_t* s) {
    return s->tree->ndata;
}

void startree_compute_inverse_perm(startree_t* s) {
    if (s->inverse_perm)
        return;
    // compute inverse permutation vector.
    s->inverse_perm = malloc(Ndata(s) * sizeof(int));
    if (!s->inverse_perm) {
        fprintf(stderr, "Failed to allocate star kdtree inverse permutation vector.\n");
        return;
    }
#ifndef NDEBUG
    {
        int i;
        for (i=0; i<Ndata(s); i++)
            s->inverse_perm[i] = -1;
    }
#endif
    kdtree_inverse_permutation(s->tree, s->inverse_perm);
#ifndef NDEBUG
    {
        int i;
        for (i=0; i<Ndata(s); i++)
            assert(s->inverse_perm[i] != -1);
    }
#endif
}

int startree_get(startree_t* s, int starid, double* posn) {
    if (s->tree->perm && !s->inverse_perm) {
        startree_compute_inverse_perm(s);
        if (!s->inverse_perm)
            return -1;
    }
    if (starid >= Ndata(s)) {
        fprintf(stderr, "Invalid star ID: %u >= %u.\n", starid, Ndata(s));
        assert(0);
        return -1;
    }
    if (s->inverse_perm) {
        kdtree_copy_data_double(s->tree, s->inverse_perm[starid], 1, posn);
    } else {
        kdtree_copy_data_double(s->tree, starid, 1, posn);
    }
    return 0;
}
