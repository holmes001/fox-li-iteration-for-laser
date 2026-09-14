#ifndef PUMP_H
#define PUMP_H

#include <complex>
#include <cmath>

#include "SimConfig.h"
/**
 * Applies gain and loss profile.
 * @param field Input/output optical field array.
 * @param beam
 */
/**
 * @param beam Pump radius and offset parameters.
 */
void gain_and_loss(grid field, beam beam);

#endif