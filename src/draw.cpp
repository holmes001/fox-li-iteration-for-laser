#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <array>   


/**
 * 颜色映射函数
 * @param value x 方向积分区间
 * @param minVal,maxVal 待转化数组的最小值和最大值
 * @param r,g,b RGB数值
 * @param colormap 映射类型（缺省为0，0=Jet,1=Hot,@=Rainbow)
 * @return  无
 */
void valueToColor(double value, double minVal, double maxVal,
                  int &r, int &g, int &b, int colormap = 0) {
    // 防止除零
    if (maxVal - minVal < 1e-12) {
        r = g = b = 0;
        return;
    }
    // 归一化到 [0,1]
    double t = (value - minVal) / (maxVal - minVal);
    t = std::max(0.0, std::min(1.0, t));  // 截断

    switch (colormap) {
        case 0: {  // Jet 色图 (蓝-青-黄-红)
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
        case 1: {  // Hot (黑-红-黄-白)
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
        default: {  // 灰度 (备用)
            int gray = static_cast<int>(t * 255.0 + 0.5);
            r = g = b = gray;
        }
    }
}

/** 
 * 创建文件
* @param data: 二维数组 (rows 行, cols 列)
* @param filename: 输出文件名 (建议 .ppm)
* @param minVal, maxVal: 数据映射范围 (若二者相等则自动计算)
* @param colormap: 色图类型 (0=Jet, 1=Hot)
*/
void drawColorMap(const std::vector<std::vector<double>>& data,
                  const std::string& filename,
                  double minVal = 0.0, double maxVal = 1.0,
                  int colormap = 0) {
    if (data.empty() || data[0].empty()) {
        std::cerr << "drawColorMap: 输入数组为空！" << std::endl;
        return;
    }

    int rows = static_cast<int>(data.size());
    int cols = static_cast<int>(data[0].size());

    // 自动计算范围（如果用户未指定）
    if (minVal == 0.0 && maxVal == 1.0) {
        // 但用户可能确实想用 0~1，无法区分。约定：若 minVal >= maxVal 则自动计算
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

    // 创建 PPM 文件 (P3 格式, 文本ASCII, 便于查看)
    std::ofstream fout(filename);
    if (!fout) {
        std::cerr << "drawColorMap: 无法创建文件 " << filename << std::endl;
        return;
    }

    // PPM 头部: P3 宽度 高度 最大颜色值
    fout << "P3\n";
    fout << cols << " " << rows << "\n";
    fout << "255\n";

    // 逐行写入像素 (注意 PPM 通常从左上开始，行从上到下)
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int r, g, b;
            valueToColor(data[i][j], minVal, maxVal, r, g, b, colormap);
            fout << r << " " << g << " " << b << " ";
        }
        fout << "\n";
    }
    fout.close();
    std::cout << "彩色图已保存为: " << filename << std::endl;
}



/**
 * 将一维数组绘制为折线图，保存为 PPM 图像（支持坐标轴）
 * @param data       输入一维数据
 * @param filename   输出文件名（建议 .ppm）
 * @param width      图像宽度（像素）
 * @param height     图像高度（像素）
 * @param minVal     纵轴最小值（若 minVal >= maxVal 则自动计算）
 * @param maxVal     纵轴最大值
 * @param lineColor  线条颜色 RGB 数组，默认黑色 {0,0,0}
 * @param bgColor    背景颜色 RGB 数组，默认白色 {255,255,255}
 * @param drawAxis   是否绘制坐标轴（默认 true）
 */
void drawCurve(const std::vector<double>& data,
               const std::string& filename,
               int width,
               int height,
               double minVal,
               double maxVal,
               const std::array<int, 3>& lineColor,
               const std::array<int, 3>& bgColor,
               bool drawAxis = true) {
    if (data.empty()) {
        std::cerr << "drawCurve: 数据为空！" << std::endl;
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

    // 创建像素数组
    std::vector<unsigned char> pixels(width * height * 3);
    // 填充背景
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int idx = (i * width + j) * 3;
            pixels[idx]     = bgColor[0];
            pixels[idx + 1] = bgColor[1];
            pixels[idx + 2] = bgColor[2];
        }
    }

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

    // 画线函数：接收颜色参数
    auto drawLine = [&](int x0, int y0, int x1, int y1, const std::array<int, 3>& color) {
        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;
        while (true) {
            if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height) {
                int idx = (y0 * width + x0) * 3;
                pixels[idx]     = color[0];
                pixels[idx + 1] = color[1];
                pixels[idx + 2] = color[2];
            }
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x0 += sx; }
            if (e2 <  dx) { err += dx; y0 += sy; }
        }
    };

    // 绘制曲线
    for (int i = 0; i < n - 1; ++i) {
        int x0 = mapX(i);
        int y0 = mapY(data[i]);
        int x1 = mapX(i + 1);
        int y1 = mapY(data[i + 1]);
        drawLine(x0, y0, x1, y1, lineColor);
    }

    // 单点处理
    if (n == 1) {
        int x = mapX(0);
        int y = mapY(data[0]);
        if (x >= 0 && x < width && y >= 0 && y < height) {
            drawLine(x, y, x, y, lineColor);  // 画一个点
        }
    }

    // ===== 绘制坐标轴 =====
    if (drawAxis) {
        const int tickLen = 6;
        const std::array<int, 3> axisColor = lineColor;  // 使用曲线颜色

        // 横轴
        drawLine(0, height - 1, width - 1, height - 1, axisColor);
        // 纵轴
        drawLine(0, 0, 0, height - 1, axisColor);

        // 横轴刻度
        int numXTicks = std::max(3, std::min(10, n / 50 + 2));
        if (n > 1) {
            for (int k = 0; k < numXTicks; ++k) {
                int idx = static_cast<int>(k * (n - 1) / (double)(numXTicks - 1) + 0.5);
                int x = mapX(idx);
                drawLine(x, height - 1, x, height - 1 - tickLen, axisColor);
            }
        } else {
            int x = width / 2;
            drawLine(x, height - 1, x, height - 1 - tickLen, axisColor);
        }

        // 纵轴刻度
        int numYTicks = 5;
        for (int k = 0; k < numYTicks; ++k) {
            double val = minVal + k * (maxVal - minVal) / (numYTicks - 1);
            int y = mapY(val);
            drawLine(0, y, tickLen, y, axisColor);
        }
    }

    // 写入 PPM 二进制文件
    std::ofstream fout(filename, std::ios::binary);
    if (!fout) {
        std::cerr << "drawCurve: 无法创建文件 " << filename << std::endl;
        return;
    }
    fout << "P6\n" << width << " " << height << "\n255\n";
    fout.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());
    fout.close();

    std::cout << "曲线图已保存为: " << filename << std::endl;
}

// 简化版本（默认颜色和尺寸）
