#ifndef SIMULATION_CONFIG_H
#define SIMULATION_CONFIG_H

#include <iostream>
#include "integral.h"

#pragma once
struct SimulationConfig {
    int N = 512;                  // 网格数
    double dx = 1e-6;            // 采样间隔 (m)
    double lambda = 0.5e-6;      // 波长 (m)
    double cavity_length = 0.1;  // 腔长 (m)
    double mirror_radius = 50e-6;// 镜面半径 (m)
    int num_iterations = 100;    // 迭代次数
    std::string output_dir = "./picture/";
};

#endif // SIMULATION_CONFIG_H