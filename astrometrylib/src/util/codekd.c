/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#include <stdio.h>
#include <stdlib.h>

#include "codekd.h"
#include "starutil.h"

static codetree_t* codetree_alloc() {
    codetree_t* s = calloc(1, sizeof(codetree_t));
    if (!s) {
        fprintf(stderr, "Failed to allocate a code kdtree struct.\n");
        return NULL;
    }
    return s;
}

int codetree_close(codetree_t* s) {
    if (!s) return 0;
    if (s->inverse_perm)
        free(s->inverse_perm);
    free(s);
    return 0;
}

static int Ndata(codetree_t* s) {
    return s->tree->ndata;
}

void codetree_compute_inverse_perm(codetree_t* s) {
    // compute inverse permutation vector.
    s->inverse_perm = malloc(Ndata(s) * sizeof(int));
    if (!s->inverse_perm) {
        fprintf(stderr, "Failed to allocate code kdtree inverse permutation vector.\n");
        return;
    }
    kdtree_inverse_permutation(s->tree, s->inverse_perm);
}

int codetree_get(codetree_t* s, unsigned int codeid, double* code) {
    if (s->tree->perm && !s->inverse_perm) {
        codetree_compute_inverse_perm(s);
        if (!s->inverse_perm)
            return -1;
    }
    if (codeid >= (unsigned)Ndata(s)) {
        fprintf(stderr, "Invalid code ID: %u >= %u.\n", codeid, Ndata(s));
        return -1;
    }
    if (s->inverse_perm)
        kdtree_copy_data_double(s->tree, s->inverse_perm[codeid], 1, code);
    else
        kdtree_copy_data_double(s->tree, codeid, 1, code);
    return 0;
}
