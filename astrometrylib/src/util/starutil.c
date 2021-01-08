/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#include "compat.h"
#include "starutil.h"
#include "mathutil.h"

double mjdtojd(double mjd) {
    return mjd + MJD_JD_OFFSET;
}
double jdtomjd(double jd) {
    return jd - MJD_JD_OFFSET;
}

double rad2deg(double x) {
    return x * DEG_PER_RAD;
}

double deg2rad(double x) {
    return x * RAD_PER_DEG;
}

double deg2arcmin(double x) {
    return x * ARCMIN_PER_DEG;
}

double arcmin2deg(double x) {
    return x * DEG_PER_ARCMIN;
}

double arcmin2arcsec(double x) {
    return x * ARCSEC_PER_ARCMIN;
}

double arcsec2arcmin(double x) {
    return x * ARCMIN_PER_ARCSEC;
}

double rad2arcmin(double x) {
    return x * ARCMIN_PER_RAD;
}

double rad2arcsec(double x) {
    return x * ARCSEC_PER_RAD;
}

double deg2arcsec(double x) {
    return x * ARCSEC_PER_DEG;
}

double arcmin2rad(double x) {
    return x * RAD_PER_ARCMIN;
}

double arcsec2rad(double x) {
    return x * RAD_PER_ARCSEC;
}

double arcsec2deg(double x) {
    return x * DEG_PER_ARCSEC;
}

double rad2distsq(double x) {
    // inverse of distsq2arc; cosine law.
    return 2.0 * (1.0 - cos(x));
}

double rad2dist(double x) {
    return sqrt(rad2distsq(x));
}

double arcsec2distsq(double x) {
    return rad2distsq(arcsec2rad(x));
}

double arcmin2dist(double x) {
    return rad2dist(arcmin2rad(x));
}

double arcmin2distsq(double arcmin) {
    return rad2distsq(arcmin2rad(arcmin));
}

double z2dec(double z) {
    return asin(z);
}

double xy2ra(double x, double y) {
    double a = atan2(y, x);
    if (a < 0)
        a += 2.0 * M_PI;
    return a;
}

void xyz2radec(double x, double y, double z, double* ra, double* dec) {
    if (ra)
        *ra = xy2ra(x, y);
    if (dec)
        *dec = z2dec(z);
}

void xyzarr2radec(const double* xyz, double* ra, double* dec) {
    xyz2radec(xyz[0], xyz[1], xyz[2], ra, dec);
}

void xyzarr2radecdeg(const double* xyz, double* ra, double* dec) {
    xyzarr2radec(xyz, ra, dec);
    if (ra) *ra = rad2deg(*ra);
    if (dec) *dec = rad2deg(*dec);
}

void xyzarr2radecdegarr(double* xyz, double* radec) {
    xyzarr2radecdeg(xyz, radec, radec + 1);
}

void radec2xyzarr(double ra, double dec, double* xyz) {
    double cosdec = cos(dec);
    xyz[0] = cosdec * cos(ra);
    xyz[1] = cosdec * sin(ra);
    xyz[2] = sin(dec);
}

void radec2xyz(double ra, double dec,
    double* x, double* y, double* z) {
    double cosdec = cos(dec);
    *x = cosdec * cos(ra);
    *y = cosdec * sin(ra);
    *z = sin(dec);
}

void radecdeg2xyz(double ra, double dec,
    double* x, double* y, double* z) {
    radec2xyz(deg2rad(ra), deg2rad(dec), x, y, z);
}

void radecdeg2xyzarr(double ra, double dec, double* xyz) {
    radec2xyzarr(deg2rad(ra), deg2rad(dec), xyz);
}

bool star_coords(const double* s, const double* r,
    bool tangent, double* x, double* y) {
    // As used by the sip.c code, this does the TAN projection
    // (if "tangent" is true; SIN projection otherwise)
    // r: CRVAL
    // s: RA,Dec to be projected
    // ASSUME r,s are unit vectors
    // sdotr:  s dot r = |r||s| cos(theta) = cos(theta)
    double sdotr = s[0] * r[0] + s[1] * r[1] + s[2] * r[2];
    if (sdotr <= 0.0) {
        // on the opposite side of the sky
        return false;
    }
    if (r[2] == 1.0) {
        // North pole
        double inv_s2 = 1.0 / s[2];
        if (tangent) {
            *x = s[0] * inv_s2;
            *y = s[1] * inv_s2;
        }
        else {
            *x = s[0];
            *y = s[1];
        }
    }
    else if (r[2] == -1.0) {
        // South pole
        double inv_s2 = 1.0 / s[2];
        if (tangent) {
            *x = -s[0] * inv_s2;
            *y = s[1] * inv_s2;
        }
        else {
            *x = -s[0];
            *y = s[1];
        }
    }
    else {
        double etax, etay, xix, xiy, xiz, eta_norm;
        double inv_en, inv_sdotr;
        // eta is a vector perpendicular to r pointing in the direction
        // of increasing RA.  eta_z = 0 by definition.
        etax = -r[1];
        etay = r[0];
        eta_norm = hypot(etax, etay);
        inv_en = 1.0 / eta_norm;
        etax *= inv_en;
        etay *= inv_en;

        // xi =  r cross eta, a vector pointing northwards,
        // in direction of increasing DEC
        xix = -r[2] * etay;
        xiy = r[2] * etax;
        xiz = r[0] * etay - r[1] * etax;

        // project s-r onto eta and xi.  No need to subtract r from s, though,
        // since eta and xi are orthogonal to r by construction.
        *x = (s[0] * etax + s[1] * etay);
        *y = (s[0] * xix + s[1] * xiy + s[2] * xiz);

        // The "inv_sdotr" applies the TAN scaling
        if (tangent) {
            inv_sdotr = 1.0 / sdotr;
            *x *= inv_sdotr;
            *y *= inv_sdotr;
        }
    }
    return true;
}

double distsq2rad(double dist2) {
    // cosine law: c^2 = a^2 + b^2 - 2 a b cos C
    // c^2 is dist2.  We want C.
    // a = b = 1
    // c^2 = 1 + 1 - 2 cos C
    // dist2 = 2( 1 - cos C )
    // 1 - (dist2 / 2) = cos C
    // C = acos(1 - dist2 / 2)
    return acos(1.0 - dist2 / 2.0);
}

double arcsec2dist(double arcInArcSec) {
    return sqrt(arcsec2distsq(arcInArcSec));
}

// Degrees to distance on the unit sphere.
double deg2dist(double arcInDegrees) {
    return arcsec2dist(deg2arcsec(arcInDegrees));
}

double deg2distsq(double d) {
    return rad2distsq(deg2rad(d));
}

double distsq2arcsec(double dist2) {
    return rad2arcsec(distsq2rad(dist2));
}

double dist2arcsec(double dist) {
    return distsq2arcsec(dist * dist);
}

double dist2deg(double dist) {
    return arcsec2deg(dist2arcsec(dist));
}

double distsq2deg(double dist2) {
    return rad2deg(distsq2rad(dist2));
}
