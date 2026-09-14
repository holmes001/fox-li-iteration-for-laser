# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Encapsulated grid, beam, and mirror configuration in the `grid`, `beam`, and `mirror` structures.
- Added structure-based parameter passing to propagation, aperture, mirror, pump, and mode-generation functions.
- Added English documentation for the main simulation modules and public functions.

### Changed
- Centralized shared simulation types and the `Complex` alias in `inc/SimConfig.h`.
- Updated the grid sampling interval to use a floating-point value so physical sampling distances such as `1e-5` are preserved.
- Improved field image scaling and output handling for generated simulation images.

### Deprecated
- Placeholder for soon‑to‑be removed features.

### Removed
- Placeholder for removed features.

### Fixed
- Resolved naming conflicts between the project `Complex` alias and OpenCV's `cv::Complex` by avoiding global OpenCV namespace imports in public headers.
- Prevented the gain-and-loss step from replacing the propagated field with a new constant field on every iteration.

### Security
- Placeholder for security fixes.

---

## [0.1.0] – 2026-09-08

### Added
- Core Fox‑Li iteration engine with angular‑spectrum propagation (`propagate_angular_spectrum`).
- Fresnel (paraxial) and exact angular‑spectrum transfer functions.
- Hard‑edge aperture truncation on both mirrors.
- Field normalisation to conserve average intensity.
- PPM image output for 2D colour maps (`drawColorMap`) and 1D cross‑section curves (`drawCurve`).
- Jet and Hot colour maps for visualisation.
- Generic 1D/2D numerical integration utilities (trapezoidal & Simpson rules).

### Changed
- Initial commit of the complete project structure.

### Fixed
- None (initial release).

---

## [0.0.1] – 2026-09-07

### Added
- Project scaffolding and basic CMake configuration.
- Placeholder source files and headers.


