#include "diffraction.h"

#include <complex>
#include <cmath>
#include <vector>
#include <cstring>
#include <fftw3.h>

#include "optic.h"
#include "SimConfig.h"

// Forward declarations for the transfer functions used by propagation.
Complex default_transfer_function(double fx, double fy, double lambda, double L, double k);
Complex fresnel_transfer_function(double fx, double fy, double lambda, double L, double k);

/**
 * Propagates a complex optical field with the angular-spectrum method.
 *
 * FFTW plans and working buffers are reused when the grid dimensions and
 * sampling interval remain unchanged.
 * @param field Input and output optical field.
 * @param beam Beam parameters containing the wavelength.
 * @param L Propagation distance in meters.
 * @param H Transfer function used in the spatial-frequency domain.
 */
void propagate_angular_spectrum(grid& field, const beam& beam, double L,
                                Complex (*H)(double, double, double, double, double)) {
    int N = field.N ;
    double dx = field.dx;
    fftw_complex* input =  reinterpret_cast<fftw_complex*>(field.field);
    double k = 2 * M_PI / beam.lambda;

    // Cache FFTW plans and working buffers across calls.
    static fftw_plan forward_plan = nullptr;
    static fftw_plan backward_plan = nullptr;
    static fftw_complex* buf_in = nullptr;
    static fftw_complex* buf_out = nullptr;
    static int prev_N = 0;
    static double prev_dx = 0.0;

    if (N != prev_N || dx != prev_dx) {
        if (forward_plan) fftw_destroy_plan(forward_plan);
        if (backward_plan) fftw_destroy_plan(backward_plan);
        if (buf_in) fftw_free(buf_in);
        if (buf_out) fftw_free(buf_out);

        prev_N = N;
        prev_dx = dx;

        // Allocate memory that satisfies FFTW alignment requirements.
        buf_in  = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N * N);
        buf_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N * N);

        // FFTW_MEASURE may overwrite the arrays while creating the plans,
        // so dedicated dynamically allocated buffers are used here.
        forward_plan  = fftw_plan_dft_2d(N, N, buf_in, buf_in, FFTW_FORWARD, FFTW_MEASURE);
        backward_plan = fftw_plan_dft_2d(N, N, buf_in, buf_out, FFTW_BACKWARD, FFTW_MEASURE);
    }

    // Copy the input field into the internal buffer.
    std::memcpy(buf_in, input, sizeof(fftw_complex) * N * N);

    // Compute the forward FFT.
    fftw_execute(forward_plan);

    // Apply the transfer function in the spatial-frequency domain.
    double inv_dx = 1.0 / (N * dx);
    std::vector<double> fx(N), fy(N);
    for (int i = 0; i < N; ++i) {
        fx[i] = (i <= N/2) ? i * inv_dx : (i - N) * inv_dx;
        fy[i] = fx[i]; // The square grid uses the same frequency coordinates.
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            Complex H_val = H(fx[i], fy[j], beam.lambda, L, k);

            double H_real = H_val.real();
            double H_imag = H_val.imag();

            // Read the real and imaginary components.
            double Fr = buf_in[i*N + j][0];
            double Fi = buf_in[i*N + j][1];

            // Multiply the spectrum by the transfer function.
            buf_in[i*N + j][0] = Fr * H_real - Fi * H_imag;
            buf_in[i*N + j][1] = Fr * H_imag + Fi * H_real;
        }
    }

    // Compute the inverse FFT.
    fftw_execute(backward_plan);

    // Normalize and copy the propagated field back to the caller.
    double norm = 1.0 / (N * N);
    for (int i = 0; i < N * N; ++i) {
        field.field[i] = {buf_out[i][0] * norm, buf_out[i][1] * norm};
    }
}


/**
 * Computes the exact angular-spectrum transfer function.
 * @param fx Spatial frequency in x.
 * @param fy Spatial frequency in y.
 * @param lambda Wavelength in meters.
 * @param L Propagation distance in meters.
 * @param k Wavenumber.
 * @return Complex transfer-function value.
 */
Complex default_transfer_function(double fx, double fy, double lambda, double L, double k) {
    double f2 = fx*fx + fy*fy;
    double arg = 1.0 - lambda*lambda * f2;
    if (arg < 0) {
        return Complex(0.0, 0.0);
    } else {
        double phase = k * L * std::sqrt(arg);
        return Complex(std::cos(phase), std::sin(phase));
    }
}

/**
 * Computes the Fresnel-approximation transfer function.
 * @param fx Spatial frequency in x.
 * @param fy Spatial frequency in y.
 * @param lambda Wavelength in meters.
 * @param L Propagation distance in meters.
 * @param k Wavenumber.
 * @return Complex transfer-function value.
 */
Complex fresnel_transfer_function(double fx, double fy, double lambda, double L, double k) {
    double f2 = fx*fx + fy*fy;
    double phase = k * L - M_PI * lambda * L * f2;
    return Complex(cos(phase), sin(phase));
}
