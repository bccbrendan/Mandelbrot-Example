#!/bin/bash
set -euo pipefail

CXXFLAGS=(--std=c++11 -g -O0)
SOURCES=(main.cpp Mandelbrot.cpp bitmap.cpp)
OUTPUT=./builds/mandelbrot-parallel

g++ "${CXXFLAGS[@]}" "$@" "${SOURCES[@]}" -o "$OUTPUT"


# use like
# ./build.sh -DMANDELBROT_USE_SVE -march=armv8.2-a+sve -o ./builds/mandelbrot-parallel-sve

# ./build.sh -DMANDELBROT_USE_SVE2 -march=armv9-a+sve2 -o ./builds/mandelbrot-parallel-sve2
