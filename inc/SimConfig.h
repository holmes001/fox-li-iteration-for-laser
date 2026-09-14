#ifndef SIMULATION_CONFIG_H
#define SIMULATION_CONFIG_H

#include <iostream>
#include <complex>

#include "integral.h"

#pragma once
using Complex = std::complex<double>;

/** Stores the dimensions, sampling interval, and complex samples of a field. */
typedef struct{
    int N;
    double dx;
    Complex* field;
}grid;

/** Stores the wavelength, size, position, and angle of a beam. */
typedef struct{
    double lambda;
    double radius;
    double x_offset;
    double y_offset;
    double theta;
}beam;

/** Stores the aperture radius and curvature radius of a mirror. */
typedef struct{
    double radius;
    double curve_radius;
}mirror;

#endif // SIMULATION_CONFIG_H