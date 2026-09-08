#ifndef DRAW_H
#define DRAW_H

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <array>

    extern std::array<int, 3> black ;
    extern std::array<int, 3> white ;
 

void valueToColor(double value, double minVal, double maxVal,
                  int &r, int &g, int &b, int colormap = 0);
void drawColorMap(const std::vector<std::vector<double>>& data,
                  const std::string& filename,
                  double minVal = 0.0, double maxVal = 1.0,
                  int colormap = 0) ;
void drawCurve(const std::vector<double>& data,
               const std::string& filename,
               int width,
               int height,
               double minVal,
               double maxVal,
               const std::array<int, 3>& lineColor,
               const std::array<int, 3>& bgColor,
               bool drawAxis = true);
inline void drawCurve(const std::vector<double>& data,
                      const std::string& filename,
                      int width = 800,
                      int height = 400,
                      double minVal = 0.0,
                      double maxVal = 1.0,
                      bool drawAxis = true) {
    std::array<int, 3> black = {0, 0, 0};
    std::array<int, 3> white = {255, 255, 255};
    drawCurve(data, filename, width, height, minVal, maxVal, black, white, drawAxis);
}
#endif  