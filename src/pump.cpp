#include "pump.h"

#include <complex>
#include <cmath>

#include "optic.h"
#include "SimConfig.h"

/**
 * Applies gain and loss profile.
 * @param field Input/output optical field array.
 * @param beam
 */
void gain_and_loss(grid field, beam beam){
    double center = (field.N - 1) / 2.0;
    for (int i = 0; i < field.N; ++i) {
        double x = (i - center) * field.dx - beam.x_offset;
        for (int j = 0; j < field.N; ++j) {
            double y = (j - center) * field.dx - beam.y_offset;
            double r2 = x * x + y * y;
            if(r2 <= beam.radius * beam.radius) {
                field.field[i*field.N + j] *= 0.6;
            } else {
                field.field[i*field.N + j] = 0.0;
            }
        }
    }
}