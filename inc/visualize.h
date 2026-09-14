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

/**
 * Maps a scalar value to an RGB color.
 * @param value Input value.
 * @param minVal Minimum value.
 * @param maxVal Maximum value.
 * @param r Output red channel.
 * @param g Output green channel.
 * @param b Output blue channel.
 * @param colormap Color map type.
 */
void valueToColor(double value, double minVal, double maxVal,
                  int &r, int &g, int &b, int colormap);

/**
 * Saves a 2D data array as a color image.
 * @param data Input 2D array.
 * @param filename Output filename.
 * @param minVal Minimum value for mapping.
 * @param maxVal Maximum value for mapping.
 * @param colormap Color map type.
 */
void drawColorMap(const vector<vector<double>>& data,
                  const string& filename,
                  double minVal, double maxVal,
                  int colormap);

/**
 * Saves a 1D data array as a curve plot.
 * @param data Input 1D array.
 * @param filename Output filename.
 * @param width Image width.
 * @param height Image height.
 * @param minVal Minimum y value.
 * @param maxVal Maximum y value.
 * @param lineColor Line color in RGB.
 * @param bgColor Background color in RGB.
 * @param drawAxis Whether to draw axes.
 */
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