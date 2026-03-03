#!/bin/bash

set -euo pipefail

MODE="${1:-release}"
CXX="${CXX:-g++}"
SRC=(main.cpp Mandelbrot.cpp bitmap.cpp)
OUT_DIR="./builds"

mkdir -p "$OUT_DIR"

case "$MODE" in
	debug)
		OUT="$OUT_DIR/mandelbrot-parallel-debug"
		CXXFLAGS=(--std=c++11 -g -O0)
		;;
	release)
		OUT="$OUT_DIR/mandelbrot-parallel"
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