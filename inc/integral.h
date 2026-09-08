#ifndef INTEGRAL_H
#define INTEGRAL_H

#include <iostream>
#include <math.h> 

// 通用梯形法则（支持 double 和 complex<double>）
/**
 * 梯形法则 (复合梯形公式)
 * @param f     被积函数指针 double f(double x)
 * @param a     积分下限
 * @param b     积分上限
 * @param n     分段数 (n > 0)
 * @return      积分近似值
 */
template<typename T, typename Func>
T trapezoidal_generic(Func f, double a, double b, int n) {
    if (n <= 0) return T(0);
    double h = (b - a) / n;
    T sum = (f(a) + f(b)) * 0.5;  // 复数支持与标量乘法
    for (int i = 1; i < n; ++i) {
        sum += f(a + i * h);
    }
    return sum * h;
}

/**
 * 辛普森法则 (复合辛普森公式，要求 n 为偶数)
 * @param f     被积函数指针 double f(double x)
 * @param a     积分下限
 * @param b     积分上限
 * @param n     分段数 (自动调整为偶数)
 * @return      积分近似值
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


/* ---------- 二维数值积分 (矩形区域) ---------- */

/**
 * 二维复合辛普森公式 (要求 nx, ny 均为偶数)
 * @param f     被积函数指针 double f(double x, double y)
 * @param x1,x2 x 方向积分区间
 * @param y1,y2 y 方向积分区间
 * @param nx,ny 两个方向的分段数 (自动调整为偶数)
 * @return      二重积分近似值
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
