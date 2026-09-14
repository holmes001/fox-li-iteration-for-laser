#ifndef INTEGRAL_H
#define INTEGRAL_H

#include <iostream>
#include <math.h> 

// Composite trapezoidal rule for both double and std::complex<double>.
/**
 * Computes a composite trapezoidal-rule integral.
 * @param f Integrand callable, such as a function of one real variable.
 * @param a Lower bound.
 * @param b Upper bound.
 * @param n Number of segments (n > 0).
 * @return Approximate integral value.
 */
template<typename T, typename Func>
T trapezoidal_generic(Func f, double a, double b, int n) {
    if (n <= 0) return T(0);
    double h = (b - a) / n;
    T sum = (f(a) + f(b)) * 0.5;  // Supports both real and complex values.
    for (int i = 1; i < n; ++i) {
        sum += f(a + i * h);
    }
    return sum * h;
}

/**
 * Computes a composite Simpson-rule integral.
 * @param f Integrand callable, such as a function of one real variable.
 * @param a Lower bound.
 * @param b Upper bound.
 * @param n Number of segments; adjusted to an even number automatically.
 * @return Approximate integral value.
 */
template<typename T, typename Func>
T simpson_generic(Func f, double a, double b, int n) {
    if (n <= 0) return T(0);
    if (n % 2 != 0) n++;
    double h = (b - a) / n;
    T sum = f(a) + f(b);
    for (int i = 1; i < n; ++i) {
        double x = a + i * h;
        sum += (i % 2 == 0) ? T(2) * f(x) : T(4) * f(x);
    }
    return sum * h / T(3);
}

/* ---------- 2D numerical integration over a rectangular region ---------- */

/**
 * Computes a two-dimensional composite Simpson-rule integral.
 * @param f Integrand callable of two real variables.
 * @param x1 Lower x bound.
 * @param x2 Upper x bound.
 * @param y1 Lower y bound.
 * @param y2 Upper y bound.
 * @param nx Number of segments in x; adjusted to an even number automatically.
 * @param ny Number of segments in y; adjusted to an even number automatically.
 * @return Approximate double integral value.
 */
template<typename T, typename Func>
T integral_2d_simpson(Func f, double x1, double x2, double y1, double y2, int nx, int ny) {
    if (nx <= 0 || ny <= 0) return T(0);
    if (nx % 2 != 0) nx++;
    if (ny % 2 != 0) ny++;

    double hx = (x2 - x1) / nx;
    double hy = (y2 - y1) / ny;
    double sum = 0.0;

    for (int i = 0; i <= nx; ++i) {
        double x = x1 + i * hx;
        double wx = (i == 0 || i == nx) ? 1.0 : (i % 2 == 0 ? 2.0 : 4.0);

        double inner_sum = 0.0;
        for (int j = 0; j <= ny; ++j) {
            double y = y1 + j * hy;
            double wy = (j == 0 || j == ny) ? 1.0 : (j % 2 == 0 ? 2.0 : 4.0);
            inner_sum += wy * f(x, y);
        }
        sum += wx * inner_sum;
    }
    return sum * hx * hy / 9.0;
}

double func1d(double x) ;
double func2d(double x, double y);

#endif  
