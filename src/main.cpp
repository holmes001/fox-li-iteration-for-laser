#include <iostream>
#include <windows.h>

#include <complex>
#include <cmath>
#include <fftw3.h>
#include <vector>

#include "diffraction.h"
#include "integral.h"
#include "optic.h"
#include "SimConfig.h"
#include "visualize.h"

using namespace std;

using Complex = std::complex<double>; 

// ======== 新增：归一化函数（保持总能量不变） ========
void normalize_field(std::complex<double>* field, int N) {
    double norm = 0.0;
    for (int i = 0; i < N*N; ++i) {
        norm += std::norm(field[i]);   // |field|^2
    }
    if (norm > 0.0) {
        double scale = 1.0 / std::sqrt(norm / (N*N)); // 使平均强度为1
        for (int i = 0; i < N*N; ++i) {
            field[i] *= scale;
        }
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "Program started." << std::endl;
    
    const int N = 1024;                // 网格数（2的幂）
    const double dx = 1e-6;           // 采样间隔 1 μm
    const double lambda = 5e-7;       // 波长 500 nm
    const double cavity_length_1 = 0.12; // 腔长 120 mm（即单程传播距离）
    const double cavity_length_2 = 0.063; // 腔长 180 mm（即单程传播距离）

    // 镜面参数：半径（假设为 50 μm，即 50 个像素）
    const double mirror_radius = 500e-6; // 5 mm
    const double beam_radius = 90e-6; // 30 μm
    const double radius = 0.1; 

    // 迭代次数
    const int num_iterations = 401;

    // ---------- 内存分配 ----------
    std::complex<double>* temp   = (std::complex<double>*)fftw_malloc(sizeof(std::complex<double>) * N * N);
    std::complex<double>* left = (std::complex<double>*)fftw_malloc(sizeof(std::complex<double>) * N * N);
    std::complex<double>* right = (std::complex<double>*)fftw_malloc(sizeof(std::complex<double>) * N * N);

    for (int i = 0; i < N*N; ++i) left[i] = 1.0;

    //gaussian_beam(left, N, dx, beam_radius);
    init_beam(left, N, dx, beam_radius);
    //apply_aperture(left, N, dx, mirror_radius);

    std::vector<std::vector<double>> matrix_1(N, std::vector<double>(N));
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            matrix_1[i][j] = std::abs(left[i*N + j]);
    drawColorMap(matrix_1, "D:\\code\\cmake\\fox-li\\picture\\picture_start.png", 0.0, 1.0,0);
    cout << "picture saved to picture_start.png" << endl;

    cout << "Initial field (uniform plane wave) set." << endl;

    
    for (int iter = 0; iter < num_iterations; ++iter) {
        
        //gain_and_loss(left, N, dx, beam_radius);
        // 1. 传播到右镜
        propagate_angular_spectrum(reinterpret_cast<fftw_complex*>(left), reinterpret_cast<fftw_complex*>(temp), N, dx,lambda,cavity_length_1,fresnel_transfer_function);

        apply_concave_mirror(temp, N, dx, radius, lambda, mirror_radius);

        propagate_angular_spectrum(reinterpret_cast<fftw_complex*>(temp), reinterpret_cast<fftw_complex*>(right), N, dx,lambda,cavity_length_2,fresnel_transfer_function);
        // 2. 应用右镜反射（圆形镜面）
        apply_aperture(right, N, dx, mirror_radius);

        propagate_angular_spectrum(reinterpret_cast<fftw_complex*>(right), reinterpret_cast<fftw_complex*>(temp), N, dx,lambda,cavity_length_1,fresnel_transfer_function);

        apply_concave_mirror(temp, N, dx, radius, lambda, mirror_radius);
        // 3. 传播回左镜
        propagate_angular_spectrum(reinterpret_cast<fftw_complex*>(temp), reinterpret_cast<fftw_complex*>(left), N, dx, lambda, cavity_length_2,fresnel_transfer_function);

        // 4. 应用左镜反射（同样为圆形镜面）
        apply_aperture(left, N, dx, mirror_radius);

        // 5. 归一化光场，保持总能量不变
        //normalize_field(left, N);

        if(iter % 100 == 0) {
            std::vector<std::vector<double>> matrix(N, std::vector<double>(N));
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    matrix[i][j] = std::abs(left[i*N + j]);
            drawColorMap(matrix, "D:\\code\\cmake\\fox-li\\picture\\picture_iter_" + std::to_string(iter) + ".png", 0.0, 1.0,0);
            cout << "Iteration " << iter << " picture saved." << endl;
        }
    }

    // 最终场输出
    cout << "Fox-Li iteration completed." << endl;
    double maxVal = 0.0, minVal = 1e30;
    for (int idx = 0; idx < N*N; ++idx) {
        double val = std::abs(left[idx]);
        if (val < minVal) minVal = val;
        if (val > maxVal) maxVal = val;
    }
    cout << "Final field max amplitude = " << maxVal << ", min = " << minVal << endl;

    // 保存最终光场图
    std::vector<std::vector<double>> matrix(N, std::vector<double>(N));
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            matrix[i][j] = std::abs(left[i*N + j]);
    drawColorMap(matrix, "D:\\code\\cmake\\fox-li\\picture\\picture_final.png", 0.0, 1.0,0);
    cout << "Final picture saved to picture_final.png" << endl;

    // 释放内存
    fftw_free(left);
    fftw_free(right);
    fftw_free(temp);
    return 0;
}