#ifndef OPTIC_H
#define OPTIC_H

#include <complex>

#include "SimConfig.h"

/** Evaluates the physicists' Hermite polynomial.
 * @param n Polynomial order.
 * @param x Input value.
 */
static double Hermite(int n, double x);
/**
 * Applies a hard-edged circular aperture.
 * @param field Input and output optical field.
 * @param mirror Mirror aperture parameters.
 */
void apply_aperture(grid& field, const mirror& mirror) ;

/**
 * Applies a Gaussian aperture.
 * @param field Input and output optical field.
 * @param mirror Mirror aperture parameters.
 */
void apply_aperture_gaussian(grid& field, const mirror& mirror);

/**
 * Applies a concave mirror phase and hard aperture.
 * @param field Input and output optical field.
 * @param beam Beam parameters containing wavelength and angle.
 * @param mirror Mirror aperture and curvature parameters.
 */
void apply_concave_mirror(grid& field, const beam& beam, const mirror& mirror);

/**
 * Generates a TEM00 mode field.
 * @param field Input and output optical field.
 * @param beam Beam waist and offset parameters.
 */
void TEM_00(grid& field, const beam& beam);

/**
 * Generates a Hermite-Gaussian mode field.
 * @param field Input and output optical field.
 * @param m Mode index in x direction.
 * @param n Mode index in y direction.
 * @param beam Beam waist and offset parameters.
 */
void HG(grid& field, const beam& beam, int m, int n);


#endif
