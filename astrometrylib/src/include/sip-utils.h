/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#ifndef SIP_UTILS_H
#define SIP_UTILS_H

#include "sip.h"

double wcs_pixel_center_for_size(double size);

/**
 sip->a_order, sip->b_order, sip->a, and sip->b must be set.

 sip->ap_order and sip->bp_order must be set.

 Computes sip->ap and sip->bp by inverting the forward polynomial evaluated
 on a regular grid of points, NX x NY, between xlo,xhi and ylo,yhi.

 If NX,NY are 0, a reasonable default will be chosen.

 If xlo=xhi=0 or ylo=yhi=0, the bounds of the image (from
 sip->wcstan.imagew/h) will be used.
 */
int sip_compute_inverse_polynomials(sip_t* sip, int NX, int NY,
                                    double xlo, double xhi,
                                    double ylo, double yhi);

/*
 Finds stars that are inside the bounds of a given field (wcs).

 One of "sip" or "tan" must be non-NULL; if "sip" is non-NULL it is used.

 One of "xyz" or "radec" must be non-NULL.  If both are non-NULL, xyz is used.
 "N" indicates how many elements are in these arrays.  "radec" are in degrees.

 If "inds" is non-NULL, the indices of stars that are inside the field are
 put there; otherwise a new int array is allocated and returned; it should
 be free()'d.

 The pixel (xy) positions are placed into a newly-allocated array at "xy",
 unless "xy" is NULL.

 The number of good stars is placed in Ngood, which must be non-NULL.
 */
int* sip_filter_stars_in_field(const sip_t* sip, const tan_t* tan,
                               const double* xyz, const double* radec,
                               int N,
                               double** xy, int* inds, int* Ngood);

/**
 Returns the bounds of the image in RA,Dec space, approximately, by
 walking the boundary of the image in steps of size "stepsize".

 Return values satisfy:
 -90 <= decmin <= decmax <= 90
 ramin <= ramax
 ramin may be < 0, or ramax > 360, if the image straddles RA=0.

 */

// sets RA,Dec in degrees.

bool sip_pixel_is_inside_image(const sip_t* wcs, double px, double py);

bool tan_pixel_is_inside_image(const tan_t* wcs, double px, double py);


#endif
