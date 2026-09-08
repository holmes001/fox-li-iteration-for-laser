#include <iostream>
#include <complex>
#include <cmath>
#include <vector>
#include <fftw3.h>

extern double k; // 波数 k = 2π/λ

// 声明默认的传递函数（将在文件末尾定义）
std::complex<double> default_transfer_function(double fx, double fy, double lambda, double z, double k);

/**
 * 角谱传播函数（单次传播），支持自定义频域传递函数
 * @param input  输入光场（二维复数数组，行优先），尺寸 N x N
 * @param output 输出光场（二维复数数组，行优先），尺寸 N x N
 * @param N      网格尺寸（通常为2的幂）
 * @param dx     采样间隔（空间域，单位m）
 * @param lambda 波长（单位m）
 * @param z      传播距离（单位m）
 * @param H      传递函数指针，形如 complex<double> H(fx, fy, lambda, z, k)，默认使用角谱传递函数
 */
void propagate_angular_spectrum(fftw_complex* input, fftw_complex* output,
                                int N, double dx, double lambda, double z,
                                std::complex<double> (*H)(double, double, double, double, double) = default_transfer_function) {
    // 1. 创建FFT计划（正向变换）
    fftw_plan forward_plan = fftw_plan_dft_2d(N, N, input, input,
                                               FFTW_FORWARD, FFTW_ESTIMATE);
    // 2. 创建IFFT计划（逆向变换）
    fftw_plan backward_plan = fftw_plan_dft_2d(N, N, input, output,
                                                FFTW_BACKWARD, FFTW_ESTIMATE);

    // 执行正向FFT
    fftw_execute(forward_plan);

    // 3. 频域传递函数相乘
    double inv_dx = 1.0 / (N * dx);
    for (int i = 0; i < N; ++i) {
        double fx = (i - N/2.0) * inv_dx;
        for (int j = 0; j < N; ++j) {
            double fy = (j - N/2.0) * inv_dx;

            // 调用用户提供的传递函数 H
            std::complex<double> H_val = H(fx, fy, lambda, z, k);
            double H_real = H_val.real();
            double H_imag = H_val.imag();

            // 获取频域值
            double Fr = input[i*N + j][0];
            double Fi = input[i*N + j][1];
            // 复数乘法： G = F * H
            double Gr = Fr * H_real - Fi * H_imag;
            double Gi = Fr * H_imag + Fi * H_real;
            input[i*N + j][0] = Gr;
            input[i*N + j][1] = Gi;
        }
    }

    // 4. 执行逆向FFT
    fftw_execute(backward_plan);

    // 5. 归一化
    double norm = 1.0 / (N * N);
    for (int i = 0; i < N * N; ++i) {
        output[i][0] *= norm;
        output[i][1] *= norm;
    }

    // 6. 清理计划
    fftw_destroy_plan(forward_plan);
    fftw_destroy_plan(backward_plan);
}

// =============== 默认传递函数（示例：角谱传递函数） ===============
/**
 * 角谱传递函数：H = exp( i * k * z * sqrt(1 - (lambda*f)^2) )
 * 当 (lambda*f)^2 > 1 时，为倏逝波，返回 0
 */
std::complex<double> default_transfer_function(double fx, double fy, double lambda, double z, double k) {
    double f2 = fx*fx + fy*fy;
    double arg = 1.0 - lambda*lambda * f2;
    if (arg < 0) {
        // 倏逝波截断
        return std::complex<double>(0.0, 0.0);
    } else {
        double phase = k * z * std::sqrt(arg);
        return std::complex<double>(std::cos(phase), std::sin(phase));
    }
}
// 这是激光物理教科书里标准的菲涅尔传递函数（傍轴近似）
std::complex<double> fresnel_transfer_function(double fx, double fy, double lambda, double z, double k) {
    double f2 = fx*fx + fy*fy;
    // 忽略 sqrt 中的高阶项，直接使用傍轴近似：exp(ikz) * exp(-i*pi*lambda*z*f^2)
    double phase = k * z - M_PI * lambda * z * f2;
    return std::complex<double>(cos(phase), sin(phase));
}