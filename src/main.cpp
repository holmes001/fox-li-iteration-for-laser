#include <iostream>
#include <windows.h>

#include <string>
#include <complex>
#include <cmath>
#include <filesystem>
#include <fftw3.h>
#include <vector>

#include "diffraction.h"
#include "integral.h"
#include "optic.h"
#include "pump.h"
#include "SimConfig.h"
#include "visualize.h"

using namespace std;

void normalize_field(grid& field) ;
void draw_picture(grid& field, const std::string& filename);

#ifndef PROJECT_PICTURE_DIR
#define PROJECT_PICTURE_DIR "picture"
#endif

/** Returns the absolute path of a file inside the output picture directory,
 *  creating the directory if necessary. */
std::string picture_path(const std::string& name) {
    namespace fs = std::filesystem;
    static const fs::path dir = [] {
        fs::path d(PROJECT_PICTURE_DIR);
        std::error_code ec;
        fs::create_directories(d, ec);   // 目录已存在时不会报错。
        return d;
    }();
    return (dir / name).string();
}

/** Runs the Fox-Li resonator simulation and writes field images to disk. */
int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "Program started." << std::endl;

    grid left;
    left.N = 512;
    left.dx =1e-5;
    left.field = (Complex*)fftw_malloc(sizeof(Complex) * left.N * left.N);

    beam pump_beam;
    pump_beam.lambda = 5e-7;
    pump_beam.radius = 2e-4;
    pump_beam.x_offset = 0.0;
    pump_beam.y_offset = 0.2e-3*sqrt(3);
    //pump_beam.y_offset = 0.0;
    pump_beam.theta = 10.0 * M_PI / 180.0;

    mirror concave_mirror;
    concave_mirror.radius = 1e-3;
    concave_mirror.curve_radius = 0.1;

    double cavity_length_1 = 0.12 ;
    double cavity_length_2 = 0.063 ;
    // Number of Fox-Li iterations.
    const int num_iterations = 15501;

    for (int i = 0; i < left.N*left.N; ++i) left.field[i] = 0.6;

    TEM_00(left, pump_beam);
    //HG(left,pump_beam,4,4);

    string filename = picture_path("picture_start.png");

    draw_picture(left, filename);

    cout << "picture saved to picture_start.png" << endl;

    cout << "Initial field (uniform plane wave) set." << endl;

    for (int iter = 0; iter < num_iterations; ++iter) {

        gain_and_loss(left, pump_beam);
        // 1. Propagate to the right mirror.
        propagate_angular_spectrum(left,pump_beam,cavity_length_1,fresnel_transfer_function);

        apply_concave_mirror(left, pump_beam, concave_mirror);

        propagate_angular_spectrum(left,pump_beam, cavity_length_2,fresnel_transfer_function);
        // 2. Apply the right-mirror reflection through the circular aperture.
        apply_aperture(left, concave_mirror);

        propagate_angular_spectrum(left,pump_beam,cavity_length_2,fresnel_transfer_function);

        apply_concave_mirror(left, pump_beam,concave_mirror);
        // 3. Propagate back to the left mirror.
        propagate_angular_spectrum(left,pump_beam, cavity_length_1,fresnel_transfer_function);

        // 4. Apply the left-mirror reflection through the same aperture.
        apply_aperture(left, concave_mirror);

        // 5. Normalize the field to keep the total energy constant.
        normalize_field(left);

        if(iter % 500 == 0) {
            string filename_1 = picture_path("picture_iter_" + std::to_string(iter) + ".png");
            draw_picture(left, filename_1);
            cout << "Iteration " << iter << " picture saved." << endl;
        }
    }

    // Write the final field output.
    cout << "Fox-Li iteration completed." << endl;

    // Save the final field image.
    draw_picture(left, picture_path("picture_final.png"));
    cout << "Final picture saved to picture_final.png" << endl;

    // Release the FFTW-allocated field buffer.
    fftw_free(left.field);
    return 0;
}

/** Normalizes the field so that its mean intensity is one. */
void normalize_field(grid& field) {
    int size = field.N*field.N;
    double norm = 0.0;
    for (int i = 0; i < size; ++i) {
        norm += std::norm(field.field[i]);   // Accumulate |field|^2.
    }
    if (norm > 0.0) {
        double scale = 1.0 / std::sqrt(norm / (size));
        for (int i = 0; i < size; ++i) {
            field.field[i] *= scale;
        }
    }
}

/** Saves the field amplitude as a color-map image. */
void draw_picture(grid& field, const std::string& filename)
{
    std::vector<std::vector<double>> matrix_1(field.N, std::vector<double>(field.N));
    for (int i = 0; i < field.N; ++i)
    {
        for (int j = 0; j < field.N; ++j)
        {
            matrix_1[i][j] = std::abs(field.field[i*field.N + j]);
        }
    }
    double maxVal = 0.0, minVal = 1e30;
        for (int idx = 0; idx < field.N*field.N; ++idx) {
            double val = std::abs(field.field[idx]);
            if (val < minVal) minVal = val;
            if (val > maxVal) maxVal = val;
        }
    drawColorMap(matrix_1, filename, minVal,maxVal, 0);
}
