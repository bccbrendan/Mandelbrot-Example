#!/bin/bash

set -euo pipefail

MODE="${1:-release}"
CXX="${CXX:-g++}"
SRC=(main.cpp Mandelbrot.cpp bitmap.cpp)
OUT_DIR="./builds"

mkdir -p "$OUT_DIR"

case "$MODE" in
	mandelbrot-sve2)
		OUT="$OUT_DIR/mandelbrot-sve2"
		CXXFLAGS=(
			--std=c++11
			-O3
            -march=armv9-a+sve2
			-mcpu=neoverse-n1+crc+crypto
			-ffast-math
			-funroll-loops
			-flto
			-DNDEBUG
		)
		;;
	mandelbrot-sve)
		OUT="$OUT_DIR/mandelbrot-sve"
		CXXFLAGS=(
			--std=c++11
			-O3
            -march=armv8.2-a+sve
			-mcpu=neoverse-n1+crc+crypto
			-ffast-math
			-funroll-loops
			-flto
			-DNDEBUG
		)
		;;
	mandelbrot-n1)
		OUT="$OUT_DIR/mandelbrot-n1"
		CXXFLAGS=(
			--std=c++11
			-O3
			-mcpu=neoverse-n1+crc+crypto
			-ffast-math
			-funroll-loops
			-flto
			-DNDEBUG
		)
		;;
	*)
		echo "Usage: $0 [debug|release]"
		exit 1
		;;
esac

echo "Building $MODE -> $OUT"
"$CXX" "${CXXFLAGS[@]}" "${SRC[@]}" -o "$OUT"
