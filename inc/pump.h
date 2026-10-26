#ifndef PUMP_H
#define PUMP_H

#include <complex>
#include <cmath>

#include "SimConfig.h"

/**
 * @brief 泵浦光与腔模的**交叠积分** J（《实验说明 HG 2026》"考虑泵浦交叠"一页）
 *
 * @details 实验讲义给出的定义（四能级系统，考虑泵浦交叠后的振荡阈值）：
 *
 *              P_pt = h·nu_p·delta / (eta_F·tau·sigma·l·J)
 *              J    = ∫_V r(x,y,z)·s(x,y,z) dV
 *
 *          其中 r 是**归一化的泵浦强度分布**，s 是**归一化的腔模强度分布**。
 *          由于泵浦与模场在 z（轴向）上的分布都近似均匀，纵向积分退化成一个
 *          常数因子，本实现对横截面做二维积分：
 *
 *              J = (∫∫ r·s dA)^2 / (∫∫ r^2 dA · ∫∫ s^2 dA)
 *
 *          这里用的是 Cauchy–Schwarz 归一化形式，保证 0 <= J <= 1：
 *              J = 1  <=>  s(x,y) 与 r(x,y) 形状完全一致（完美交叠）
 *              J -> 0  <=>  两者几乎没有重叠
 *          讲义中 r、s 已归一化为单位体积分，那种写法与本式只差一个常数因子；
 *          该因子会并入增益系数 g0，**模式的排序不受影响**。
 *
 * @note 物理含义（讲义原文）："当泵浦光偏离谐振腔光轴时，与泵浦光具有最大的
 *       交叠积分的模式具有最高的增益 —— 最低的阈值。"
 *       所以 J 是**模式选择判据**：对同一个泵浦位置，算出各阶模的 J，
 *       J 最大的那一阶最先起振。
 *
 * @param field 输入光场（只读）
 * @param beam  泵浦参数（radius / x_offset / y_offset）
 * @return 交叠积分 J，范围 [0,1]；光场为空或泵浦分布为零时返回 0
 */
double overlap_integral(const grid& field, const beam& beam);

/**
 * @brief 施加泵浦增益（按泵浦强度分布逐点作用）
 *
 * @details 强度增益取 g(x,y) = g0·r(x,y)，幅度因子为 sqrt(exp(g)) = exp(g/2)，
 *          其中 r(x,y) 是峰值归一化的泵浦强度分布（见 pump_profile）。
 *
 *          为什么是"逐点按 r(x,y)"而不是"全场乘 J"：
 *          任何模式拿到的**总**功率增益正比于
 *              ∫ g(x,y)·s(x,y) dA / ∫ s(x,y) dA  ∝  ∫ r·s dA
 *          也就是 `overlap_integral()` 分子里的那个积分。所以逐点按 r 加增益，
 *          就等于"交叠积分越大的模式增益越高"（讲义判据），而且它是**线性**的：
 *          增益不依赖当前场形，Fox-Li 迭代的收敛结果与初值无关。
 *
 *          若直接把全场乘 exp(g0·J/2)：由于 main.cpp 每轮都会调用
 *          normalize_field()，一个全局标量会被完全抵消，等于什么都没做。
 *
 * @note 泵浦圆斑之外 r -> 0，增益因子 -> 1，即"没有增益"而不是"完全损耗"。
 *       真正的损耗由镜面孔径（apply_aperture / apply_concave_mirror）承担。
 *
 * @param field 输入/输出光场
 * @param beam  泵浦参数
 */
void gain_and_loss(grid& field, const beam& beam);

#endif
