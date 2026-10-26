#include "optic.h"

#include <complex>
#include <cmath>

#include "integral.h"
#include "SimConfig.h"

/**
 * Hermite polynomial.
 * @param n Polynomial order.
 * @param x Input value.
 */
static double Hermite(int n, double x) {
    if (n == 0) return 1.0;
    if (n == 1) return 2.0 * x;
    double H0 = 1.0;
    double H1 = 2.0 * x;
    double Hn = 0.0;
    for (int i = 2; i <= n; ++i) {
        Hn = 2.0 * x * H1 - 2.0 * (i - 1) * H0;
        H0 = H1;
        H1 = Hn;
    }
    return Hn;
}

/**
 * Applies a hard-edged circular aperture.
 * @param field Input/output optical field array..
 * @param mirror
 */
void apply_aperture(grid& field, const mirror& mirror)  {
    double center = (field.N - 1) / 2.0;          // Pixel-grid center.
    for (int i = 0; i < field.N; ++i) {
        for (int j = 0; j < field.N; ++j) {
            double x = (i - center) * field.dx;
            double y = (j - center) * field.dx;
            double r = std::sqrt(x*x + y*y);
            if (r > mirror.radius) {
                field.field[i*field.N + j] = 0.0;
            }
        }
    }
}

/**
 * Applies a Gaussian aperture.
 * @param field Input/output optical field array.
 * @param mirror_radius Gaussian aperture radius.
 */
void apply_aperture_gaussian(grid& field, const mirror& mirror) {
    double center = (field.N - 1) / 2.0;          // Pixel-grid center.
    for (int i = 0; i < field.N; ++i) {
        for (int j = 0; j < field.N; ++j) {
            double x = (i - center) * field.dx;
            double y = (j - center) * field.dx;
            double r = std::sqrt(x*x + y*y);
            field.field[i*field.N+j] *= std::exp(-pow(r / mirror.radius, 8));
        }
    }
}


/**
 * Applies a concave mirror phase and hard aperture.
 * @param field Input/output optical field array.
 * @param beam
 * @param mirror
 */
void apply_concave_mirror(grid& field, const beam& beam, const mirror& mirror) {
    double k = 2.0 * M_PI / beam.lambda;
    double center = (field.N - 1) / 2.0;

    // Astigmatism gives different tangential and sagittal curvature radii.
    double cos_t = std::cos(beam.theta);
    double R_x = mirror.curve_radius * cos_t;       // Tangential curvature radius.
    double R_y = mirror.curve_radius / cos_t;       // Sagittal curvature radius.

    for (int i = 0; i < field.N; ++i) {
        double x = (i - center) * field.dx;
        for (int j = 0; j < field.N; ++j) {
            double y = (j - center) * field.dx;
            double r2 = x * x + y * y;
            double r = std::sqrt(r2);
            int idx = i * field.N + j;

            // Zero the field outside the mirror aperture to model diffraction loss.
            if (r > mirror.radius) {
                field.field[idx] = Complex(0.0, 0.0);
            } else {
                // Use separate curvature radii for the x and y phase terms.
                // A spherical mirror uses -k * r^2 / R.
                // The astigmatic mirror uses -k * (x^2 / R_x + y^2 / R_y).
                double phase = -k * (x * x / R_x + y * y / R_y);
                Complex mirror_phase(std::cos(phase), std::sin(phase));
                field.field[idx] *= mirror_phase;
            }
        }
    }
}

/**
 * Generates a TEM00 mode field.
 * @param field Input/output optical field array.
 * @param beam
 */
void TEM_00(grid& field, const beam& beam){
    double center = (field.N - 1) / 2.0;
    for (int i = 0; i < field.N; ++i) {
        double x = (i - center) * field.dx;
        for (int j = 0; j < field.N; ++j) {
            double y = (j - center) * field.dx;
            double r2 = (x - beam.x_offset) * (x - beam.x_offset) + (y - beam.y_offset) * (y - beam.y_offset);
            field.field[i*field.N + j] *= std::exp(-r2 / (beam.radius * beam.radius));
        }
    }
}

/**
 * Generates a Hermite-Gaussian mode field.
 * @param field
 * @param m Mode index in x direction.
 * @param n Mode index in y direction.
 * @param beam
 */
void HG(grid& field, const beam& beam, int m, int n) {
    double center = (field.N - 1) / 2.0;
    for (int i = 0; i < field.N; ++i) {
        double x = (i - center) * field.dx;
        for (int j = 0; j < field.N; ++j) {
            double y = (j - center) * field.dx;
            double r2 = (x - beam.x_offset) * (x - beam.x_offset) + (y - beam.y_offset) * (y - beam.y_offset);
            double hg_factor = Hermite(m, std::sqrt(2.0) * (x - beam.x_offset) / beam.radius)
                 * Hermite(n, std::sqrt(2.0) * (y - beam.y_offset) / beam.radius);
            field.field[i*field.N + j] *= 0.6* hg_factor * std::exp(-r2 / (beam.radius * beam.radius));
        }
    }
}
