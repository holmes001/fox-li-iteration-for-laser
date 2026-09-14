# Fox‑Li Iteration for Laser Cavity Mode Simulation

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![FFTW3](https://img.shields.io/badge/FFTW-3.3.10-green.svg)](http://www.fftw.org/)

**Fox‑Li iteration** is a classic numerical method to find the steady‑state transverse modes of a laser resonator. This project implements the iterative propagation of an optical field between two mirrors, applying aperture truncation and normalisation at each round trip. The code is written in **C++17** and uses **FFTW3** for fast Fourier transforms.

---

## Features

- **Angular‑spectrum propagation** – exact (with evanescent‑wave cut‑off) and Fresnel (paraxial) transfer functions.
- **Arbitrary aperture shapes** – currently supports circular hard‑edge mirrors (easily extensible).
- **Real‑time visualisation** – saves 2D intensity maps and 1D cross‑sections as **PPM images** during iteration.
- **Colour maps** – Jet and Hot palettes for intuitive visualisation.
- **Numerical integration** – generic 1D and 2D trapezoidal/Simpson integrators (supports both `double` and `std::complex<double>`).

---

## Program Structure

- `inc/` contains public headers and shared simulation types.
- `src/` contains the Fox-Li iteration, optical propagation, mirror and beam operations, numerical integration, and image output.
- `picture/` stores field images generated during the simulation.
- `build/` is the CMake build directory and is generated locally.
- `CMakeLists.txt` defines the C++17 executable and links FFTW3 and OpenCV.

The shared simulation configuration is defined in `inc/SimConfig.h`:

- `grid` stores the grid size, sampling interval, and complex field samples.
- `beam` stores the wavelength, beam radius, transverse offsets, and incidence angle.
- `mirror` stores the aperture radius and curvature radius.

These structures are passed to the propagation, pump, aperture, mirror, and mode-generation functions. This keeps related parameters together, reduces long parameter lists, and makes the interfaces easier to extend.

The main simulation loop initializes a field, applies the pump profile, propagates the field between the cavity mirrors, applies mirror phase and aperture effects, normalizes the field, and periodically saves field images.

---

## Dependencies

- **C++17** (or later) compiler (GCC, Clang, MSVC).
- **FFTW3** – [http://www.fftw.org/](http://www.fftw.org/) (install via package manager or from source).
- **CMake** 3.10+ (for building).

---

## Build Instructions

1. **Clone the repository**
   ```bash
   git clone https://github.com/holmes001/fox-li-iteration-for-laser.git
   cd fox-li-iteration-for-laser
2. **Configure with CMake**
   ```bash 
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
3. **Compile**
   ```bash 
   cmake --build . --config Release
4. **Run**
   ```bash 
   ./fox-li-iteration.exe 