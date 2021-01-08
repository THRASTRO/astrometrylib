/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#ifndef STAR_KD_H
#define STAR_KD_H

#include "compat.h"
#include "kdtree.h"

typedef struct {
    kdtree_t* tree;
    int* inverse_perm;
    uint8_t* sweep;
} startree_t;

/**
 Searches for stars within a radius of a point.

 xyzcenter: double[3]: unit-sphere coordinates of point; see
 starutil.h : radecdeg2xyzarr() to convert RA,Decs to this form.

 radius2: radius-squared on the unit sphere;
 see starutil.h: deg2distsq() or arcsec2distsq().

 xyzresults: if non-NULL, returns the xyz positions of the
 stars that are found, in a newly-allocated array.

 radecresults: if non-NULL, returns the RA,Dec
 positions (in degrees) of the stars within range.

 starinds: if non-NULL, returns the indices of stars within range.
 This can be used to retrieve extra information about the stars,
 using the 'startree_get_data_column()' function.
 
 */
void startree_search_for(const startree_t* s, const double* xyzcenter, double radius2,
                         double** xyzresults, double** radecresults,
                         int** starinds, int* nresults);

int startree_get(startree_t* s, int starid, double* posn);

int startree_close(startree_t* s);

void startree_compute_inverse_perm(startree_t* s);

#endif
