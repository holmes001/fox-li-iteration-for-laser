#ifndef DIFFRACTION_H
#define DIFRACTION_H

#include <complex>
#include <fftw3.h>

#include "SimConfig.h"

/**
 * Default transfer function for angular-spectrum propagation.
 * @param fx Spatial frequency in x.
 * @param fy Spatial frequency in y.
 * @param lambda Wavelength.
 * @param z Propagation distance.
 * @param k Wavenumber.
 * @return Transfer function value.
 */
Complex default_transfer_function(double fx, double fy, double lambda, double z, double k);

/**
 * Fresnel transfer function.
 * @param fx Spatial frequency in x.
 * @param fy Spatial frequency in y.
 * @param lambda Wavelength.
 * @param z Propagation distance.
 * @param k Wavenumber.
 * @return Transfer function value.
 */
Complex fresnel_transfer_function(double fx, double fy, double lambda, double z, double k);

/**
 * Propagates a field with the angular-spectrum method.
 * @param field Input and output optical field.
 * @param beam Beam parameters containing the wavelength.
 * @param L Propagation distance in meters.
 * @param H Transfer-function callback.
 */
void propagate_angular_spectrum(grid field, beam beam,double L,
                                Complex (*H)(double, double, double, double, double));

#endif  // DIFRACTION_H