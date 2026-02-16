## Mandelbrot Example

This repository provides a portable C++11 implementation for generating images of the Mandelbrot set—a well-known fractal commonly used to demonstrate complex-number math, iterative algorithms, and performance characteristics in compute-heavy workloads.

The project renders the fractal into a bitmap image. The output resolution is configured at compile time, making it easy to build consistent artifacts and compare results across different systems, compilers, and optimization settings.

**Please Note**: This codebase is intended for learning and demonstration purposes. It is not designed, reviewed, or supported as production software, and it may omit features that production implementations typically require (for example: robust input validation, extensive configurability, and comprehensive error handling). The source code is provided under the Arm Education license.

## To Build

Ensure you have a C++ compiler available. The build script, `./build.sh` is intended for Linux-based platforms. 


Original Author: Kieran Hejmadi, Software and Academic Ecosystem Manager, Arm