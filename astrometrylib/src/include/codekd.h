/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#ifndef CODE_KD_H
#define CODE_KD_H

#include "compat.h"
#include "kdtree.h"

typedef struct {
    kdtree_t* tree;
    int* inverse_perm;
} codetree_t;

int codetree_get(codetree_t* s, unsigned int codeid, double* code);

int codetree_close(codetree_t* s);

#endif
