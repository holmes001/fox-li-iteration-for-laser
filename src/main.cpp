#include <iostream>
#include "integral.h"
#include "app.h"
#include "draw.h"
#include <windows.h>
#include <complex>
#include <cmath>
#include <fftw3.h>
#include <vector>

using namespace std;

// 全局常数（原已定义）
double k = 2 * M_PI / 0.5;          // 波数，λ=0.5 μm
double L = 0.1;                     // 距离参数（未使用，但保留）

using Complex = std::complex<double>;

// ======== 新增：硬边光阑截断函数 ========
void apply_aperture(std::complex<double>* field, int N, double dx, double mirror_radius) {
    double center = (N - 1) / 2.0;          // 像素中心
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            double x = (i - center) * dx;
            double y = (j - center) * dx;
            double r = std::sqrt(x*x + y*y);
            if (r > mirror_radius) {
                field[i*N + j] = 0.0;
            }
        }
    }
}

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

    // ---------- 物理参数 ----------
    const int N = 512;                // 网格数（2的幂）
    const double dx = 1e-6;           // 采样间隔 1 μm
    const double lambda = 0.5e-6;     // 波长 0.5 μm
    const double cavity_length = 0.1; // 腔长 0.1 m（即单程传播距离）

    // 镜面参数：半径（假设为 50 μm，即 50 个像素）
    const double mirror_radius = 50e-6; // 50 μm

    // 迭代次数
    const int num_iterations = 100;

    // ---------- 内存分配 ----------
    std::complex<double>* field = (std::complex<double>*)fftw_malloc(sizeof(std::complex<double>) * N * N);
    std::complex<double>* temp   = (std::complex<double>*)fftw_malloc(sizeof(std::complex<double>) * N * N);

    // ---------- 初始场：均匀平面波（振幅1，相位0） ----------
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            field[i*N + j] = 0.5;
        }
    }
    cout << "Initial field (uniform plane wave) set." << endl;

    // ---------- Fox-Li 迭代循环 ----------
    for (int iter = 0; iter < num_iterations; ++iter) {
        // 1. 从左镜传播到右镜（正向）
        propagate_angular_spectrum(
            reinterpret_cast<fftw_complex*>(field),
            reinterpret_cast<fftw_complex*>(temp),
            N, dx, lambda, cavity_length,
            fresnel_transfer_function   // 使用菲涅尔近似传递函数
        );
        // 复制回 field
        for (int i = 0; i < N*N; ++i) field[i] = temp[i];

        // 2. 在右镜上施加硬边光阑（截断）
        apply_aperture(field, N, dx, mirror_radius);

        // 3. 从右镜传播回左镜（反向，距离相同）
        propagate_angular_spectrum(
            reinterpret_cast<fftw_complex*>(field),
            reinterpret_cast<fftw_complex*>(temp),
            N, dx, lambda, cavity_length,
            fresnel_transfer_function
        );
        for (int i = 0; i < N*N; ++i) field[i] = temp[i];

        // 4. 在左镜上施加同样的光阑截断
        apply_aperture(field, N, dx, mirror_radius);

        // 5. （可选）归一化，防止数值溢出
        normalize_field(field, N);

        // 6. 每 10 步输出一次光场图，观察收敛
        if ((iter + 1) % 10 == 0 || iter == 0) {
            // 构建 matrix 用于绘图
            std::vector<std::vector<double>> matrix(N, std::vector<double>(N));
            std::vector<double> curve(N);
            double maxVal = 0.0;
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    double amp = std::abs(field[i*N + j]);
                    matrix[i][j] = amp;
                    if (amp > maxVal) maxVal = amp;
                    if(i == N/2) curve[j] = amp; // 中心横截面
                }
            }
            cout<<curve[N/2]<<endl;
            // 归一化到 [0,1] 显示
            if (maxVal > 0) {
                for (int i = 0; i < N; ++i){
                    for (int j = 0; j < N; ++j){
                        matrix[i][j] /= maxVal;   
                    }
                    curve[i] /= maxVal;
                }
            }
            char filename_1[256];
            char filename_2[256];
            sprintf(filename_1, "D:\\code\\cmake\\fox-li\\picture\\picture_iter_%03d.ppm", iter+1);
            sprintf(filename_2, "D:\\code\\cmake\\fox-li\\picture\\picture_curve_%03d.ppm", iter+1);
            drawColorMap(matrix, filename_1, 0.0, 1.0);
            drawCurve(curve, filename_2, 800, 400, 0.0, 1.0);
            cout << "Iteration " << (iter+1) << " saved to " << filename_1   << endl;
        }
    }

    // 最终场输出
    cout << "Fox-Li iteration completed." << endl;
    double maxVal = 0.0, minVal = 1e30;
    for (int idx = 0; idx < N*N; ++idx) {
        double val = std::abs(field[idx]);
        if (val < minVal) minVal = val;
        if (val > maxVal) maxVal = val;
    }
    cout << "Final field max amplitude = " << maxVal << ", min = " << minVal << endl;

    // 保存最终光场图
    std::vector<std::vector<double>> matrix(N, std::vector<double>(N));
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            matrix[i][j] = std::abs(field[i*N + j]);
    drawColorMap(matrix, "D:\\code\\cmake\\fox-li\\picture\\picture_final.ppm", 0.0, 1.0);
    cout << "Final picture saved to picture_final.ppm" << endl;

    // 释放内存
    fftw_free(field);
    fftw_free(temp);
    return 0;
}