/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#include "compat.h"
#include "matchobj.h"

void matchobj_compute_derived(MatchObj* mo) {
    int mx;
    int i;
    mx = 0;
    for (i=0; i<mo->dimquads; i++)
        mx = MAX(mx, (int)mo->field[i]);
    mo->objs_tried = mx+1;
    if (mo->wcs_valid)
        mo->scale = tan_pixel_scale(&(mo->wcstan));
    mo->radius = deg2dist(mo->radius_deg);
    mo->nbest = mo->nmatch + mo->ndistractor + mo->nconflict;
}
