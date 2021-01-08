/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#ifndef ONEFIELD_H
#define ONEFIELD_H

#include "compat.h"
#include "solver.h"
#include "matchobj.h"
#include "starutil.h"

#define DEFAULT_QSF_LO 0.1
#define DEFAULT_QSF_HI 1.0

struct onefield_params {
    solver_t solver;

    // Quad size fraction: select indexes that contain quads of size fraction
    // [quad_size_fraction_lo, quad_size_fraction_hi] of the image size.
    double quad_size_fraction_lo;
    double quad_size_fraction_hi;

    uint64_t cpulimit;
    uint64_t cpu_start;
    bool hit_cpulimit;

    uint64_t timelimit;
    uint64_t time_start;
    bool hit_timelimit;

    uint64_t total_cpulimit;
    uint64_t cpu_total_start;
    bool hit_total_cpulimit;

    uint64_t total_timelimit;
    uint64_t time_total_start;
    bool hit_total_timelimit;

    // filename for cancelling
    // char* cancelfname;
    bool cancelled;

};
typedef struct onefield_params onefield_t;


void onefield_solve(solver_t* sp);

void onefield_init(onefield_t* bp);

#endif
