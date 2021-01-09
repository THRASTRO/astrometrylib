/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#include "compat.h"
#include "onefield.h"

void onefield_init(onefield_t* bp)
{
    memset(bp, 0, sizeof(onefield_t));
    bp->quad_size_fraction_lo = DEFAULT_QSF_LO;
    bp->quad_size_fraction_hi = DEFAULT_QSF_HI;

}

void onefield_solve(solver_t* sp)
{

    // Take initial time-stamps (ms)
    sp->start_tcpu = get_cpu_usage();
    sp->start_twall = get_wall_time();

    if (!sp->fieldxy_orig) {
        logerr("Failed to read xylist field.\n");
        return;
    }

    // Re-use solver object
    solver_reset_counters(sp);
    solver_reset_best_match(sp);
    solver_preprocess_field(sp);

    logverb("Solving field now.\n");
    sp->distance_from_quad_bonus = true;
    solver_log_params(sp);

    // The real thing
    solver_run(sp);

    logverb("Field %i: tried %i quads, matched %i codes.\n",
        0, sp->numtries, sp->nummatches);

    if (sp->maxquads && sp->numtries >= sp->maxquads)
        logmsg("  exceeded the number of quads to try: %i >= %i.\n",
            sp->numtries, sp->maxquads);
    if (sp->maxmatches && sp->nummatches >= sp->maxmatches)
        logmsg("  exceeded the number of quads to match: %i >= %i.\n",
            sp->nummatches, sp->maxmatches);

    // if (verify_wcs) {
    //     logmsg("Verifying WCS of field %i.\n", 0);
    //     solver_verify_sip_wcs(sp, verify_wcs); //, &mo);
    //     logmsg(" --> log-odds %g\n", sp->best_logodds);
    // }

    if (sp->best_match_solves) {

        if (sp->cancel) *sp->cancel = true;

        uint64_t delta_cpu = get_cpu_usage() - sp->start_tcpu;
        uint64_t delta_wall = get_wall_time() - sp->start_twall;
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        printf("Field solved in %.2fs (used %.2fs cpu time).\n",
            delta_wall / 1000.0, delta_cpu / 1000.0);
        printf("INDEX: %s\n", sp->index->indexfn);
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    }
    else if (sp->cancel && *sp->cancel) {

        uint64_t delta_cpu = get_cpu_usage() - sp->start_tcpu;
        uint64_t delta_wall = get_wall_time() - sp->start_twall;
        printf("Field canceled after %.2fs (used %.2fs cpu time).\n",
            delta_wall / 1000.0, delta_cpu / 1000.0);
        printf("INDEX: %s\n", sp->index->indexfn);

    }
    else {

        // Field unsolved.
        uint64_t delta_cpu = get_cpu_usage() - sp->start_tcpu;
        uint64_t delta_wall = get_wall_time() - sp->start_twall;
        printf("----------------------------------------------------\n");
        printf("Field failed after %.2fs (used %.2fs cpu time).\n",
            delta_wall / 1000.0, delta_cpu / 1000.0);
        printf("Did %d code searches, and %d star field matches.\n",
            sp->code_searches, sp->star_searches);
        printf("INDEX: %s\n", sp->index->indexfn);
        printf("----------------------------------------------------\n");

        if (sp->have_best_match) {
            logverb("Best match encountered: %g\n", exp(sp->best_logodds));
            //matchobj_print(&(sp->best_match), log_get_level());
        } else {
            logverb("Best odds encountered: %g\n", exp(sp->best_logodds));
        }
    }

    sp->fieldxy_orig->x = 0;
    sp->fieldxy_orig->y = 0;
    solver_free_field(sp);

}
