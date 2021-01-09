/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_vector_double.h>

#include "compat.h"
#include "sip-utils.h"
#include "gslutils.h"
#include "mathutil.h"

void sip_get_radec_center(const sip_t* wcs,
    double* p_ra, double* p_dec) {
    double px = wcs_pixel_center_for_size(wcs->wcstan.imagew);
    double py = wcs_pixel_center_for_size(wcs->wcstan.imageh);
    sip_pixelxy2radec(wcs, px, py, p_ra, p_dec);
}

double wcs_pixel_center_for_size(double size) {
    return 0.5 + 0.5 * size;
}

int sip_compute_inverse_polynomials(sip_t* sip, int NX, int NY,
                                    double xlo, double xhi,
                                    double ylo, double yhi) {
    int inv_sip_order;
    int M, N;
    int i, j, p, q, gu, gv;
    double maxu, maxv, minu, minv;
    double u, v, U, V;
    gsl_matrix *mA;
    gsl_vector *b1, *b2, *x1, *x2;
    tan_t* tan;

    assert(sip->a_order == sip->b_order);
    assert(sip->ap_order == sip->bp_order);
    tan = &(sip->wcstan);

    logverb("sip_compute-inverse_polynomials: A %i, AP %i\n",
            sip->a_order, sip->ap_order);

    /*
     basic idea: lay down a grid in image, for each gridpoint, push
     through the polynomial to get yourself into warped image
     coordinate (but not yet lifted onto the sky).  Then, using the
     set of warped gridpoints as inputs, fit back to their original
     grid locations as targets.
     */
    inv_sip_order = sip->ap_order;

    // Number of grid points to use:
    if (NX == 0)
        NX = 10 * (inv_sip_order + 1);
    if (NY == 0)
        NY = 10 * (inv_sip_order + 1);
    if (xhi == 0)
        xhi = tan->imagew;
    if (yhi == 0)
        yhi = tan->imageh;

    logverb("NX,NY %i,%i, x range [%f, %f], y range [%f, %f]\n",
            NX,NY, xlo, xhi, ylo, yhi);

    // Number of coefficients to solve for:
    // We only compute the upper triangle polynomial terms
    N = (inv_sip_order + 1) * (inv_sip_order + 2) / 2;

    // Number of samples to fit.
    M = NX * NY;

    mA = gsl_matrix_alloc(M, N);
    b1 = gsl_vector_alloc(M);
    b2 = gsl_vector_alloc(M);
    assert(mA);
    assert(b1);
    assert(b2);

    /*
     *  Rearranging formula (4), (5), and (6) from the SIP paper gives the
     *  following equations:
     * 
     *    +----------------------- Linear pixel coordinates in PIXELS
     *    |                        before SIP correction
     *    |                   +--- Intermediate world coordinates in DEGREES
     *    |                   |
     *    v                   v
     *                   -1
     *    U = [CD11 CD12]   * x
     *    V   [CD21 CD22]     y
     * 
     *    +---------------- PIXEL distortion delta from telescope to
     *    |                 linear coordinates
     *    |    +----------- Linear PIXEL coordinates before SIP correction
     *    |    |       +--- Polynomial U,V terms in powers of PIXELS
     *    v    v       v
     * 
     *    -f(u1,v1) =  p11 p12 p13 p14 p15 ... * ap1
     *    -f(u2,v2) =  p21 p22 p23 p24 p25 ...   ap2
     *    ...
     * 
     *    -g(u1,v1) =  p11 p12 p13 p14 p15 ... * bp1
     *    -g(u2,v2) =  p21 p22 p23 p24 p25 ...   bp2
     *    ...
     * 
     *  which recovers the A and B's.
     */

    minu = xlo - tan->crpix[0];
    maxu = xhi - tan->crpix[0];
    minv = ylo - tan->crpix[1];
    maxv = yhi - tan->crpix[1];
	
    // Sample grid locations.
    i = 0;
    for (gu=0; gu<NX; gu++) {
        for (gv=0; gv<NY; gv++) {
            double fuv, guv;
            // Calculate grid position in original image pixels
            u = (gu * (maxu - minu) / (NX-1)) + minu;
            v = (gv * (maxv - minv) / (NY-1)) + minv;
            // compute U=u+f(u,v) and V=v+g(u,v)
            sip_calc_distortion(sip, u, v, &U, &V);
            fuv = U - u;
            guv = V - v;
            // Polynomial terms...
            j = 0;
            for (p = 0; p <= inv_sip_order; p++)
                for (q = 0; q <= inv_sip_order; q++) {
                    if (p + q > inv_sip_order)
                        continue;
                    assert(j < N);
                    gsl_matrix_set(mA, i, j,
                                   pow(U, (double)p) * pow(V, (double)q));
                    j++;
                }
            assert(j == N);
            gsl_vector_set(b1, i, -fuv);
            gsl_vector_set(b2, i, -guv);
            i++;
        }
    }
    assert(i == M);

    // Solve the linear equation.
    if (gslutils_solve_leastsquares_v(mA, 2, b1, &x1, NULL, b2, &x2, NULL)) {
        ANERROR("Failed to solve SIP inverse matrix equation!");
        return -1;
    }

    // Extract the coefficients
    j = 0;
    for (p = 0; p <= inv_sip_order; p++)
        for (q = 0; q <= inv_sip_order; q++) {
            if ((p + q > inv_sip_order))
                continue;
            assert(j < N);
            sip->ap[p][q] = gsl_vector_get(x1, j);
            sip->bp[p][q] = gsl_vector_get(x2, j);
            j++;
        }
    assert(j == N);

    // Check that we found values that actually invert the polynomial.
    // The error should be particularly small at the grid points.
    if (true) {
        // rms error accumulators:
        double sumdu = 0;
        double sumdv = 0;
        int Z;
        for (gu = 0; gu < NX; gu++) {
            for (gv = 0; gv < NY; gv++) {
                double newu, newv;
                // Calculate grid position in original image pixels
                u = (gu * (maxu - minu) / (NX-1)) + minu;
                v = (gv * (maxv - minv) / (NY-1)) + minv;
                sip_calc_distortion(sip, u, v, &U, &V);
                sip_calc_inv_distortion(sip, U, V, &newu, &newv);
                sumdu += square(u - newu);
                sumdv += square(v - newv);
            }
        }
        sumdu /= (NX*NY);
        sumdv /= (NX*NY);
        debug("RMS error of inverting a distortion (at the grid points, in pixels):\n");
        debug("  du: %g\n", sqrt(sumdu));
        debug("  dv: %g\n", sqrt(sumdu));
        debug("  dist: %g\n", sqrt(sumdu + sumdv));

        sumdu = 0;
        sumdv = 0;
        Z = 1000;
        for (i=0; i<Z; i++) {
            double newu, newv;
            u = uniform_sample(minu, maxu);
            v = uniform_sample(minv, maxv);
            sip_calc_distortion(sip, u, v, &U, &V);
            sip_calc_inv_distortion(sip, U, V, &newu, &newv);
            sumdu += square(u - newu);
            sumdv += square(v - newv);
        }
        sumdu /= Z;
        sumdv /= Z;
        debug("RMS error of inverting a distortion (at random points, in pixels):\n");
        debug("  du: %g\n", sqrt(sumdu));
        debug("  dv: %g\n", sqrt(sumdu));
        debug("  dist: %g\n", sqrt(sumdu + sumdv));
    }

    gsl_matrix_free(mA);
    gsl_vector_free(b1);
    gsl_vector_free(b2);
    gsl_vector_free(x1);
    gsl_vector_free(x2);

    return 0;
}

bool tan_pixel_is_inside_image(const tan_t* wcs, double x, double y) {
    return (x >= 1 && x <= wcs->imagew && y >= 1 && y <= wcs->imageh);
}

bool sip_pixel_is_inside_image(const sip_t* wcs, double x, double y) {
    return tan_pixel_is_inside_image(&(wcs->wcstan), x, y);
}

int* sip_filter_stars_in_field(const sip_t* sip, const tan_t* tan,
                               const double* xyz, const double* radec,
                               int N, double** p_xy, int* inds, int* p_Ngood) {
    int i, Ngood;
    int W, H;
    double* xy = NULL;
    bool allocd = false;
	
    assert(sip || tan);
    assert(xyz || radec);
    assert(p_Ngood);

    Ngood = 0;
    if (!inds) {
        inds = malloc(N * sizeof(int));
        allocd = true;
    }

    if (p_xy)
        xy = malloc(N * 2 * sizeof(double));

    if (sip) {
        W = (int)sip->wcstan.imagew;
        H = (int)sip->wcstan.imageh;
    } else {
        W = (int)tan->imagew;
        H = (int)tan->imageh;
    }

    for (i=0; i<N; i++) {
        double x, y;
        if (xyz) {
            if (sip) {
                if (!sip_xyzarr2pixelxy(sip, xyz + i*3, &x, &y))
                    continue;
            } else {
                if (!tan_xyzarr2pixelxy(tan, xyz + i*3, &x, &y))
                    continue;
            }
        } else {
            if (sip) {
                if (!sip_radec2pixelxy(sip, radec[i*2], radec[i*2+1], &x, &y))
                    continue;
            } else {
                if (!tan_radec2pixelxy(tan, radec[i*2], radec[i*2+1], &x, &y))
                    continue;
            }
        }
        // FIXME -- check half- and one-pixel FITS issues.
        if ((x < 0) || (y < 0) || (x >= W) || (y >= H))
            continue;

        inds[Ngood] = i;
        if (xy) {
            xy[Ngood * 2 + 0] = x;
            xy[Ngood * 2 + 1] = y;
        }
        Ngood++;
    }

    if (allocd)
        inds = realloc(inds, Ngood * sizeof(int));

    if (xy)
        xy = realloc(xy, Ngood * 2 * sizeof(double));
    if (p_xy)
        *p_xy = xy;

    *p_Ngood = Ngood;
	
    return inds;
}
