/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#ifndef AN_MEMINDEX_H
#define AN_MEMINDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "index.h"
 
 // #include "solver.h"
// #include "pquad.h"
// #include "kdtree.h"
// #include "codekd.h"
// #include "starkd.h"
// #include "permutedsort.h"
// #include "quad-utils.h"
// #include "log.h"

void anidx_load(const char* fname, index_t* index);

#endif