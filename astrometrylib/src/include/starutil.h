/*
# This file is part of the Astrometry.net suite.
# Licensed under a 3-clause BSD style license - see LICENSE
*/

#ifndef STARUTIL_H
#define STARUTIL_H

#include <math.h>

#include "compat.h"

#define DIM_STARS 3
#define DIM_XY 2

#define ONE_OVER_SIXTY 0.016666666666666666

// pi / 180.
#define RAD_PER_DEG 0.017453292519943295
// pi / (180. * 60.)
#define RAD_PER_ARCMIN 0.00029088820866572158
// pi / (180. * 60. * 60.)
#define RAD_PER_ARCSEC 4.8481368110953598e-06

// 180. / pi
#define DEG_PER_RAD 57.295779513082323
#define DEG_PER_ARCMIN ONE_OVER_SIXTY
// 1./3600.
#define DEG_PER_ARCSEC 0.00027777777777777778

// 60. * 180. / pi
#define ARCMIN_PER_RAD 3437.7467707849396
#define ARCMIN_PER_DEG 60.0
#define ARCMIN_PER_ARCSEC ONE_OVER_SIXTY

// 60. * 60. * 180. / pi
#define ARCSEC_PER_RAD 206264.80624709636
#define ARCSEC_PER_DEG 3600.0
#define ARCSEC_PER_ARCMIN 60.0

double rad2deg(double x);
double rad2arcmin(double x);
double rad2arcsec(double x);

double deg2rad(double x);
double deg2arcmin(double x);
double deg2arcsec(double x);

double arcmin2rad(double x);
double arcmin2deg(double x);
double arcmin2arcsec(double x);

double arcsec2rad(double x);
double arcsec2deg(double x);
double arcsec2arcmin(double x);

#define MJD_JD_OFFSET 2400000.5

double mjdtojd(double mjd);
double jdtomjd(double jd);

// RA,Dec in radians:
#define radec2x(r,d) (cos(d)*cos(r))
#define radec2y(r,d) (cos(d)*sin(r))
#define radec2z(r,d) (sin(d))
double xy2ra(double x, double y);
double z2dec(double z);

// RA,Dec in radians:
void radec2xyz(double ra, double dec, double* x, double* y, double* z);
void xyz2radec(double x, double y, double z, double *ra, double *dec);
void xyzarr2radec(const double* xyz, double *ra, double *dec);
void radec2xyzarr(double ra, double dec, double* p_xyz);
void radec2xyzarrmany(double *ra, double *dec, double* xyz, int n);

// RA,Dec in degrees:
void radecdeg2xyz(double ra, double dec, double* x, double* y, double* z);
void xyzarr2radecdeg(const double* xyz, double *ra, double *dec);
void xyzarr2radecdegarr(double* xyz, double *radec);
void radecdeg2xyzarr(double ra, double dec, double* p_xyz);
void radecdegarr2xyzarr(double* radec, double* xyz);
void radecdeg2xyzarrmany(double *ra, double *dec, double* xyz, int n);

// Converts a distance-squared between two points on the
// surface of the unit sphere into the angle between the
// rays from the center of the sphere to the points, in
// radians.
double distsq2arc(double dist2);

// Distance^2 on the unit sphere to radians.
// (alias of distsq2arc)
double distsq2rad(double dist2);

double distsq2deg(double dist2);

// Distance on the unit sphere to radians.
double dist2rad(double dist);

// Distance^2 on the unit sphere to arcseconds.
double distsq2arcsec(double dist2);

// Distance on the unit sphere to arcseconds
double dist2arcsec(double dist);

// Radians to distance^2 on the unit sphere.
// (alias of arc2distsq)
double rad2distsq(double arcInRadians);

// Radians to distance on the unit sphere.
double rad2dist(double arcInRadians);

// Converts an angle (in arcseconds) into the distance-squared
// between two points on the unit sphere separated by that angle.
double arcsec2distsq(double arcInArcSec);

// Arcseconds to distance on the unit sphere.
double arcsec2dist(double arcInArcSec);

// Degrees to distance on the unit sphere.
double deg2dist(double arcInDegrees);

double deg2distsq(double d);

double arcmin2dist(double arcmin);

double arcmin2distsq(double arcmin);

// Distance on the unit sphere to degrees.
double dist2deg(double dist);

#define HELP_ERR -101
#define OPT_ERR -201

/* 
 Computes the 2D coordinates (x,y) (in units of a celestial sphere of
 radius 1) that star s would have in a TANGENTIAL PROJECTION defined
 by (centred at) star r.  s and r are both given in xyz coordinates,
 the parameters are pointers to arrays of size 3.

 If "tangent" is true, the projection is the WCS TAN projection; if
 not, it is the WCS SIN projection.

 The resulting x,y coordinates are intermediate world coordinates in
 degrees.  The "x" direction should be positive for increasing RA, "y"
 is increasing Dec.

 WARNING -- this code assumes s and r are UNIT vectors (ie normalized
 to have length 1).
*/
bool star_coords(const double *s, const double *r, 
				 bool tangent, double *x, double *y);

void star_midpoint(double* mid, const double* A, const double* B);

#endif
