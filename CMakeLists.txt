cmake_minimum_required(VERSION 3.22)
project(Shazam)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/lib)

# locate system packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW REQUIRED IMPORTED_TARGET fftw3f)

#  Python + NumPy  ➜  gives us Python.h, libpython, NumPy headers
find_package(Python3 COMPONENTS Development NumPy REQUIRED)

add_executable(Shazam main.cpp
        source/fft.cpp
        include/fft.h
        source/wavProcessing.cpp
        include/wavProcessing.h
        include/db.h
        source/db.cpp
        include/encoding.h
        source/encoding.cpp
        httpServer/cppServer.cpp
        httpServer/cppServer.h
        source/recogniser.cpp
        include/recogniser.h
)

# Tell the compiler where to look for headers
target_include_directories(Shazam PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/matplotlibcpp                   #  ➜ matplotlibcpp.h lives here
        ${CMAKE_SOURCE_DIR}/lib/cpp-httplib
        ${Python3_INCLUDE_DIRS}                     #  ➜ Python.h
        ${Python3_NumPy_INCLUDE_DIRS}               #  ➜ NumPy C-API
)

add_library(sqlite3 STATIC lib/sqlite3.c)

# Link libraries that pkg-config / CMake found
target_link_libraries(Shazam PRIVATE
        PkgConfig::FFTW
        Python3::Python
        sqlite3
        pthread #for httplib
)