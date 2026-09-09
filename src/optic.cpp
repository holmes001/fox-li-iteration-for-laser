#include "optic.h"

#include <complex>
#include <cmath>

#include "integral.h"
/**
 * 
 * @param field         输入/输出光场（std::complex数组，行优先，N*N）
 * @param N             网格尺寸
 * @param dx            空间采样间隔 (m)
 * @param mirror_radius 镜面的物理半径 (m, 硬边光阑)
 */
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

/**
 * 
 * @param field         输入/输出光场（std::complex数组，行优先，N*N）
 * @param N             网格尺寸
 * @param dx            空间采样间隔 (m)
 * @param mirror_radius 镜面的物理半径 (m, 硬边光阑)
 */
void apply_aperture_gaussian(std::complex<double>* field, int N, double dx, double mirror_radius) {
    double center = (N - 1) / 2.0;          // 像素中心
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            double x = (i - center) * dx;
            double y = (j - center) * dx;
            double r = std::sqrt(x*x + y*y);
            field[i*N+j] *= std::exp(-pow(r / mirror_radius, 8));
        }
    }
}

/**
 * 模拟凹面镜反射（硬边光阑 + 二次相位调制）
 * @param field         输入/输出光场（std::complex数组，行优先，N*N）
 * @param N             网格尺寸
 * @param dx            空间采样间隔 (m)
 * @param R             凹面镜曲率半径 (m, R>0为会聚凹面，R<0为发散凸面)
 * @param lambda        波长 (m)
 * @param mirror_radius 镜面的物理半径 (m, 硬边光阑)
 */
void apply_concave_mirror(std::complex<double>* field, int N, double dx, 
                          double R, double lambda, double mirror_radius) {
    double k = 2.0 * M_PI / lambda;
    double center = (N - 1) / 2.0;

    for (int i = 0; i < N; ++i) {
        double x = (i - center) * dx;
        for (int j = 0; j < N; ++j) {
            double y = (j - center) * dx;
            double r2 = x * x + y * y;
            double r = std::sqrt(r2);
            
            int idx = i * N + j;

            // 1. 硬边光阑：超出镜面半径的光场直接衰减为0（产生衍射损耗）
            if (r > mirror_radius) {
                field[idx] = std::complex<double>(0.0, 0.0);
            } else {
                // 2. 凹面镜的傍轴相位因子：exp(-i * k * r^2 / R)
                // 注意：根据 exp(i(kz - wt)) 传播约定，会聚凹面镜相位为负
                double phase = -k * r2 / R; 
                std::complex<double> mirror_phase(std::cos(phase), std::sin(phase));
                
                // 原场乘以相位
                field[idx] *= mirror_phase;
            }
        }
    }
}

/**
 * mode TEM_00
 * @param field         输入/输出光场（std::complex数组，行优先，N*N）
 * @param N             网格尺寸
 * @param dx            空间采样间隔 (m)
 * @param mirror_radius 镜面的物理半径 (m, 硬边光阑)
 */
void TEM_00(std::complex<double>* field, int N, double dx, double beam_radius) {
    double center = (N - 1) / 2.0;
    for (int i = 0; i < N; ++i) {
        double x = (i - center) * dx;
        for (int j = 0; j < N; ++j) {
            double y = (j - center) * dx;
            double r2 = x * x + y * y;
            field[i*N + j] *= std::exp(-r2 / (beam_radius * beam_radius));
        }
    }
}

/**
 * initialize a uniform circular beam (top-hat profile)
 * @param field         输入/输出光场（std::complex数组，行优先，N*N）
 * @param N             网格尺寸
 * @param dx            空间采样间隔 (m)
 * @param mirror_radius 镜面的物理半径 (m, 硬边光阑)
 */
void init_beam(std::complex<double>* field, int N, double dx, double beam_radius){
    double center = (N - 1) / 2.0;
    for (int i = 0; i < N; ++i) {
        double x = (i - center) * dx;
        for (int j = 0; j < N; ++j) {
            double y = (j - center) * dx;
            double r2 = x * x + y * y;
            if(r2 <= beam_radius * beam_radius) {
                field[i*N + j] = 1.0;
            } else {
                field[i*N + j] = 0.0;
            }
        }
    }
}

/**
 * 
 * @param field         输入/输出光场（std::complex数组，行优先，N*N）
 * @param N             网格尺寸
 * @param dx            空间采样间隔 (m)
 * @param mirror_radius 镜面的物理半径 (m, 硬边光阑)
 */
void gain_and_loss(std::complex<double>* field, int N, double dx, double beam_radius){
    double center = (N - 1) / 2.0;
    for (int i = 0; i < N; ++i) {
        double x = (i - center) * dx;
        for (int j = 0; j < N; ++j) {
            double y = (j - center) * dx;
            double r2 = x * x + y * y;
            if(r2 <= beam_radius * beam_radius) {
                field[i*N + j] *= 1;
            } 
            else if (r2 < 64e-10) 
            {
                field[i*N + j] *= 1; // 中心强损耗
            }
            else {
                field[i*N + j] *= 1;
            }
        }
    }
}