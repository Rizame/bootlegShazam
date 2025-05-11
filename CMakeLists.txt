cmake_minimum_required(VERSION 3.22)
project(Shazam)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/lib)

# locate system packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW REQUIRED IMPORTED_TARGET fftw3f)

add_executable(Shazam main.cpp
        source/wavProcessing.cpp
        lib/wavProcessing.h)

# Link FFTW
target_link_libraries(Shazam PRIVATE PkgConfig::FFTW)