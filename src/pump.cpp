#include "pump.h"

#include <cmath>
#include <complex>

#include "SimConfig.h"

namespace {

/**
 * @brief 单程峰值强度增益系数（小信号）
 * @note  r(x,y)=1 处（泵浦中心）的强度增益为 exp(kGainCoefficient)；
 *        换成幅度就是 exp(kGainCoefficient/2) ≈ 1.35。
 *        这个数只影响"每轮涨多少倍"，由于 main.cpp 每轮都归一化，它不改变
 *        最终收敛到的模式形状 —— 想做阈值/模式竞争必须另加饱和项。
 */
constexpr double kGainCoefficient = 0.6;

/** @brief 网格索引 i 对应的横向坐标 [m]（以网格中心为原点） */
inline double grid_coordinate(const grid& field, int i)
{
    return (i - (field.N - 1) / 2.0) * field.dx;
}

/**
 * @brief 泵浦光的相对强度分布 r(x,y)，峰值归一化到 1
 * @details 高斯泵浦，中心在 (x_offset, y_offset)，radius 为 1/e^2 强度半径。
 *          这里按圆对称处理（不含斜入射的光斑拉长）；若要把泵浦的像散足迹
 *          也算进去，只需把 x 方向的尺度改成 radius/cos(theta)。
 */
inline double pump_profile(double x, double y, const beam& beam)
{
    const double xr = (x - beam.x_offset) / beam.radius;
    const double yr = (y - beam.y_offset) / beam.radius;
    return std::exp(-2.0 * (xr * xr + yr * yr));
}

/** @brief 梯形积分权重：边界点半权重 */
inline double trapezoid_weight(int k, int n)
{
    return (k == 0 || k == n - 1) ? 0.5 : 1.0;
}

} // namespace

double overlap_integral(const grid& field, const beam& beam)
{
    double rs = 0.0;   // ∫∫ r·s dA
    double rr = 0.0;   // ∫∫ r^2 dA
    double ss = 0.0;   // ∫∫ s^2 dA

    for (int i = 0; i < field.N; ++i) {
        const double x  = grid_coordinate(field, i);
        const double wi = trapezoid_weight(i, field.N);
        for (int j = 0; j < field.N; ++j) {
            const double y = grid_coordinate(field, j);
            const double w = wi * trapezoid_weight(j, field.N);

            const double r = pump_profile(x, y, beam);                 // 泵浦强度 r(x,y)
            const double s = std::norm(field.field[i * field.N + j]);  // 腔模强度 s = |E|^2

            rs += w * r * s;
            rr += w * r * r;
            ss += w * s * s;
        }
    }

    const double den = rr * ss;
    if (den <= 0.0) {
        return 0.0;
    }
    // Cauchy–Schwarz 归一化，保证 J ∈ [0,1]，且与光场整体幅度无关。
    return (rs * rs) / den;
}

void gain_and_loss(grid& field, const beam& beam)
{
    for (int i = 0; i < field.N; ++i) {
        const double x = grid_coordinate(field, i);
        for (int j = 0; j < field.N; ++j) {
            const double y = grid_coordinate(field, j);

            // 局部强度增益 g(x,y) = g0 · r(x,y)；幅度因子 = exp(g/2)。
            const double intensity_gain = kGainCoefficient * pump_profile(x, y, beam);
            field.field[i * field.N + j] *= std::exp(0.5 * intensity_gain);
        }
    }
}
