cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type" FORCE)
endif()

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW REQUIRED fftw3)


set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/diffraction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/optic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pump.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/integral.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/visualize.cpp
)

# include files
include_directories(
    ${FFTW_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_executable(${PROJECT_NAME} ${SOURCES})

# 输出图片目录的绝对路径，避免程序依赖运行时的工作目录。
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_PICTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/picture"
)

target_link_libraries(${PROJECT_NAME} ${FFTW_LIBRARIES} ${OpenCV_LIBS})
