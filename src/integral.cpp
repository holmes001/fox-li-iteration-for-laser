#include "integral.h"

#include <iostream>

#include <complex>
#include <cmath>   
#include <functional>

/* ---------- 测试函数 (示例) ---------- */

// 一维测试函数：f(x) = x^2
double func1d(double x) {
    return x * x;
}

// 二维测试函数：f(x,y) = x*y
double func2d(double x, double y) {
    return x * y;
}

