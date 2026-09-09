#ifndef OPTIC_H
#define OPTIC_H

#include <complex>

void apply_aperture(std::complex<double>* field, int N, double dx, double mirror_radius) ;
void apply_aperture_gaussian(std::complex<double>* field, int N, double dx, double mirror_radius);
void apply_concave_mirror(std::complex<double>* field, int N, double dx, 
                          double R, double lambda, double mirror_radius);
void gaussian_beam(std::complex<double>* field, int N, double dx, double beam_radius) ;
void init_beam(std::complex<double>* field, int N, double dx, double beam_radius) ;
void gain_and_loss(std::complex<double>* field, int N, double dx, double beam_radius);
#endif 