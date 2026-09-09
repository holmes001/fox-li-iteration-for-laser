#ifndef DIFFRACTION_H
#define DIFRACTION_H

#include <complex>
#include <fftw3.h>

// 声明默认传递函数（角谱传递函数）
std::complex<double> default_transfer_function(double fx, double fy, double lambda, double z, double k);
std::complex<double> fresnel_transfer_function(double fx, double fy, double lambda, double z, double k);
/**
 * 角谱传播函数，支持自定义传递函数
 * @param H 传递函数指针，默认使用角谱传递函数
 */
void propagate_angular_spectrum(fftw_complex* input, fftw_complex* output,
                                int N, double dx, double lambda, double z,
                                std::complex<double> (*H)(double, double, double, double, double));

#endif  // DIFRACTION_H