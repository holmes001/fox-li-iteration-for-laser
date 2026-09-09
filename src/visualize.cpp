#include "visualize.h"

#include <iostream>

#include <array>
#include <algorithm>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

/**
 * color mapping function
 * @param value interval in x direction
 * @param minVal,maxVal minimum and maximum values of the array
 * @param r,g,b RGB values
 * @param colormap mapping type (default 0, 0=Jet,1=Hot,@=Rainbow)
 * @return  无
 */
void valueToColor(double value, double minVal, double maxVal,
                  int &r, int &g, int &b, int colormap = 0) {
    if (maxVal - minVal < 1e-12) {
        r = g = b = 0;
        return;
    }
    double t = (value - minVal) / (maxVal - minVal);
    t = std::max(0.0, std::min(1.0, t));

    switch (colormap) {
        case 0: {  // Jet
            double rv, gv, bv;
            if (t < 0.125) {
                rv = 0.0;
                gv = 0.0;
                bv = 0.5 + 0.5 * t / 0.125;
            } else if (t < 0.375) {
                rv = 0.0;
                gv = (t - 0.125) / 0.25;
                bv = 1.0;
            } else if (t < 0.625) {
                rv = (t - 0.375) / 0.25;
                gv = 1.0;
                bv = 1.0 - (t - 0.375) / 0.25;
            } else if (t < 0.875) {
                rv = 1.0;
                gv = 1.0 - (t - 0.625) / 0.25;
                bv = 0.0;
            } else {
                rv = 1.0 - 0.5 * (t - 0.875) / 0.125;
                gv = 0.0;
                bv = 0.0;
            }
            r = static_cast<int>(rv * 255.0 + 0.5);
            g = static_cast<int>(gv * 255.0 + 0.5);
            b = static_cast<int>(bv * 255.0 + 0.5);
            break;
        }
        case 1: {  // Hot
            double rv, gv, bv;
            if (t < 0.333) {
                rv = t / 0.333;
                gv = 0.0;
                bv = 0.0;
            } else if (t < 0.667) {
                rv = 1.0;
                gv = (t - 0.333) / 0.333;
                bv = 0.0;
            } else {
                rv = 1.0;
                gv = 1.0;
                bv = (t - 0.667) / 0.333;
            }
            r = static_cast<int>(rv * 255.0 + 0.5);
            g = static_cast<int>(gv * 255.0 + 0.5);
            b = static_cast<int>(bv * 255.0 + 0.5);
            break;
        }
        default: {  // 灰度
            int gray = static_cast<int>(t * 255.0 + 0.5);
            r = g = b = gray;
        }
    }
}

/** 
 * create graphical file from 2D array
 * @param data: 2D array (rows, cols)
 * @param filename: output filename (suggest .ppm)
 * @param minVal, maxVal: data mapping range (if equal, auto-calculated)
 * @param colormap: color map type (0=Jet, 1=Hot)
 */
void drawColorMap(const vector<vector<double>>& data,
                  const string& filename,
                  double minVal = 0.0, double maxVal = 1.0,
                  int colormap = 0) {
    if (data.empty() || data[0].empty()) {
        cerr << "drawColorMap: 输入数组为空！" << endl;
        return;
    }

    int rows = static_cast<int>(data.size());
    int cols = static_cast<int>(data[0].size());

    // 自动计算范围（与原逻辑保持一致）
    if (minVal == 0.0 && maxVal == 1.0) {
        if (minVal >= maxVal) {
            double mn = data[0][0], mx = data[0][0];
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    mn = std::min(mn, data[i][j]);
                    mx = std::max(mx, data[i][j]);
                }
            }
            if (mx - mn < 1e-12) {
                mn -= 0.5;
                mx += 0.5;
            }
            minVal = mn;
            maxVal = mx;
        }
    }

    // 创建 OpenCV 图像矩阵 (BGR)
    Mat image(rows, cols, CV_8UC3);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int r, g, b;
            valueToColor(data[i][j], minVal, maxVal, r, g, b, colormap);
            // OpenCV 存储顺序为 BGR
            image.at<Vec3b>(i, j) = Vec3b(b, g, r);
        }
    }

    // 保存图像
    if (!imwrite(filename, image)) {
        cerr << "drawColorMap: 无法保存文件 " << filename << endl;
    } else {
        cout << "彩色图已保存为: " << filename << endl;
    }
}

/**
 * create graphical file from 1D array
 * @param data       input 1D array
 * @param filename   output filename (suggest .ppm)
 * @param width      image width (pixels)
 * @param height     image height (pixels)
 * @param minVal     minimum value of the y-axis (if minVal >= maxVal, auto-calculate)
 * @param maxVal     maximum value of the y-axis
 * @param lineColor  line color RGB array, default black {0,0,0}
 * @param bgColor    background color RGB array, default white {255,255,255}
 * @param drawAxis   whether to draw coordinate axes (default true)
 */
void drawCurve(const vector<double>& data,
               const string& filename,
               int width,
               int height,
               double minVal,
               double maxVal,
               const array<int, 3>& lineColor,
               const array<int, 3>& bgColor,
               bool drawAxis = true) {
    if (data.empty()) {
        cerr << "drawCurve: 数据为空！" << endl;
        return;
    }

    int n = static_cast<int>(data.size());

    // 自动计算数值范围
    if (minVal >= maxVal) {
        double mn = data[0], mx = data[0];
        for (double v : data) {
            mn = std::min(mn, v);
            mx = std::max(mx, v);
        }
        if (mx - mn < 1e-12) {
            mn -= 0.5;
            mx += 0.5;
        }
        minVal = mn;
        maxVal = mx;
    }
    double range = maxVal - minVal;

    // 创建图像并填充背景（BGR）
    Mat image(height, width, CV_8UC3, Scalar(bgColor[2], bgColor[1], bgColor[0]));

    // 坐标映射函数
    auto mapX = [&](int idx) -> int {
        if (n == 1) return width / 2;
        return static_cast<int>(idx * (width - 1) / (double)(n - 1) + 0.5);
    };
    auto mapY = [&](double val) -> int {
        if (range < 1e-12) return height / 2;
        double t = (val - minVal) / range;
        t = std::max(0.0, std::min(1.0, t));
        return static_cast<int>((1.0 - t) * (height - 1) + 0.5);
    };

    // 转换颜色为 BGR Scalar
    Scalar lineBGR(lineColor[2], lineColor[1], lineColor[0]);

    // 绘制曲线
    for (int i = 0; i < n - 1; ++i) {
        Point p1(mapX(i), mapY(data[i]));
        Point p2(mapX(i + 1), mapY(data[i + 1]));
        line(image, p1, p2, lineBGR, 1, LINE_AA);
    }

    // 单点处理：绘制一个点（用一个小圆或单像素线）
    if (n == 1) {
        Point p(mapX(0), mapY(data[0]));
        circle(image, p, 1, lineBGR, -1, LINE_AA);
    }

    // 绘制坐标轴
    if (drawAxis) {
        const int tickLen = 6;
        Scalar axisBGR = lineBGR;

        // 横轴和纵轴
        line(image, Point(0, height - 1), Point(width - 1, height - 1), axisBGR, 1);
        line(image, Point(0, 0), Point(0, height - 1), axisBGR, 1);

        // 横轴刻度
        int numXTicks = std::max(3, std::min(10, n / 50 + 2));
        if (n > 1) {
            for (int k = 0; k < numXTicks; ++k) {
                int idx = static_cast<int>(k * (n - 1) / (double)(numXTicks - 1) + 0.5);
                int x = mapX(idx);
                line(image, Point(x, height - 1), Point(x, height - 1 - tickLen), axisBGR, 1);
            }
        } else {
            int x = width / 2;
            line(image, Point(x, height - 1), Point(x, height - 1 - tickLen), axisBGR, 1);
        }

        // 纵轴刻度
        int numYTicks = 5;
        for (int k = 0; k < numYTicks; ++k) {
            double val = minVal + k * (maxVal - minVal) / (numYTicks - 1);
            int y = mapY(val);
            line(image, Point(0, y), Point(tickLen, y), axisBGR, 1);
        }
    }

    // 保存图像
    if (!imwrite(filename, image)) {
        cerr << "drawCurve: 无法保存文件 " << filename << endl;
    } else {
        cout << "曲线图已保存为: " << filename << endl;
    }
}