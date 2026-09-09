#include "diffraction.h"

#include <complex>
#include <cmath>
#include <vector>
#include <cstring>
#include <fftw3.h>
#include "optic.h"

// 声明默认的传递函数
std::complex<double> default_transfer_function(double fx, double fy, double lambda, double L, double k);
std::complex<double> fresnel_transfer_function(double fx, double fy, double lambda, double L, double k);

void propagate_angular_spectrum(fftw_complex* input, fftw_complex* output,
                                int N, double dx, double lambda, double L,
                                std::complex<double> (*H)(double, double, double, double, double)) {
    
    double k = 2 * M_PI / lambda;

    // ======== 核心优化：缓存 FFTW 计划和缓冲区 ========
    // 利用 static 变量，只有第一次调用或者 N,dx 改变时才重新创建计划
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

        // 分配符合 FFTW 对齐要求的内存
        buf_in  = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N * N);
        buf_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N * N);

        // 改用 FFTW_MEASURE 模式（初始慢，但执行极快）
        // 注意：FFTW_MEASURE 在创建计划时会覆盖数组数据，所以用动态分配的内存
        forward_plan  = fftw_plan_dft_2d(N, N, buf_in, buf_in, FFTW_FORWARD, FFTW_MEASURE);
        backward_plan = fftw_plan_dft_2d(N, N, buf_in, buf_out, FFTW_BACKWARD, FFTW_MEASURE);
    }

    // 1. 将用户传入的 input 拷贝到内部缓冲区，避免原地修改用户数组，保证数据安全
    std::memcpy(buf_in, input, sizeof(fftw_complex) * N * N);

    // 2. 正向FFT
    fftw_execute(forward_plan);

    // 3. 频域传递函数乘法
    double inv_dx = 1.0 / (N * dx);
    std::vector<double> fx(N), fy(N);
    for (int i = 0; i < N; ++i) {
        fx[i] = (i <= N/2) ? i * inv_dx : (i - N) * inv_dx;
        fy[i] = fx[i]; // 方阵，频率坐标是对称的
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            std::complex<double> H_val = H(fx[i], fy[j], lambda, L, k);
            
            double H_real = H_val.real();
            double H_imag = H_val.imag();

            // 取复数实部和虚部
            double Fr = buf_in[i*N + j][0];
            double Fi = buf_in[i*N + j][1];
            
            // 复数乘法
            buf_in[i*N + j][0] = Fr * H_real - Fi * H_imag;
            buf_in[i*N + j][1] = Fr * H_imag + Fi * H_real;
        }
    }

    // 4. 逆向FFT
    fftw_execute(backward_plan);

    // 5. 归一化并复制回用户的 output 数组
    double norm = 1.0 / (N * N);
    for (int i = 0; i < N * N; ++i) {
        output[i][0] = buf_out[i][0] * norm;
        output[i][1] = buf_out[i][1] * norm;
    }
}

// =============== 保留你的传递函数 ===============
std::complex<double> default_transfer_function(double fx, double fy, double lambda, double L, double k) {
    double f2 = fx*fx + fy*fy;
    double arg = 1.0 - lambda*lambda * f2;
    if (arg < 0) {
        return std::complex<double>(0.0, 0.0);
    } else {
        double phase = k * L * std::sqrt(arg);
        return std::complex<double>(std::cos(phase), std::sin(phase));
    }
}

std::complex<double> fresnel_transfer_function(double fx, double fy, double lambda, double L, double k) {
    double f2 = fx*fx + fy*fy;
    double phase = k * L - M_PI * lambda * L * f2;
    return std::complex<double>(cos(phase), sin(phase));
}