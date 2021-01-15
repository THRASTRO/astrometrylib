/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#include <string.h>
#include <stdlib.h>

#include "compat.h"
#include "index.h"
#include "healpix.h"
#include "starutil.h"

bool index_overlaps_scale_range(index_t* meta,
                                  double quadlo, double quadhi) {
    bool rtn = 
        !((quadlo > meta->index_scale_upper) ||
          (quadhi < meta->index_scale_lower));
    debug("index_overlaps_scale_range: index %s has quads [%g, %g] arcsec; image has quads [%g, %g] arcsec.  In range? %s\n",
          meta->indexname, meta->index_scale_lower, meta->index_scale_upper, quadlo, quadhi, rtn ? "yes" : "no");
    return rtn;
}

bool index_is_within_range(index_t* meta, double ra, double dec, double radius_deg) {
    if (meta->healpix == -1) {
        // allsky; tautology
        return true;
    }
    return (healpix_distance_to_radec(meta->healpix, meta->hpnside, ra, dec, NULL) <= radius_deg);
}

int index_dimquads(index_t* indx) {
    return indx->dimquads;
}

void index_unload(index_t* index) {
    if (index->starkd) {
        startree_close(index->starkd);
        index->starkd = NULL;
    }
    if (index->codekd) {
        free(index->codekd);
        index->codekd = NULL;
    }
    if (index->quads) {
        index->quads = NULL;
    }
}

void index_close(index_t* index) {
}

void index_free(index_t* index) {
    index_close(index);
    free(index);
}
