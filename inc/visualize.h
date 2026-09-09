#ifndef VISUALIZE_H
#define VISUALIZE_H

#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <array>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void valueToColor(double value, double minVal, double maxVal,
                  int &r, int &g, int &b, int colormap);
void drawColorMap(const vector<vector<double>>& data,
                  const string& filename,
                  double minVal, double maxVal,
                  int colormap);
void drawCurve(const vector<double>& data,
               const string& filename,
               int width,
               int height,
               double minVal,
               double maxVal,
               const array<int, 3>& lineColor,
               const array<int, 3>& bgColor,
               bool drawAxis);
#endif