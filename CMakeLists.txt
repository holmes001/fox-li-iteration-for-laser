cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用 pkg-config 支持
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW REQUIRED fftw3)   # 自动设置 FFTW_INCLUDE_DIRS 和 FFTW_LIBRARIES

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Rectangle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/integral.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app.cpp
)

# 包含头文件
include_directories(
    ${FFTW_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# 收集源文件（保留您的 GLOB，但建议显式列出）
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

# 链接库（${FFTW_LIBRARIES} 包含 -lfftw3 等）
target_link_libraries(${PROJECT_NAME} ${FFTW_LIBRARIES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)