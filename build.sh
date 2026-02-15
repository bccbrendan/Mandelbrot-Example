#!/bin/bash

g++ --std=c++11 -O2 -Wall main.cpp bitmap.cpp Mandelbrot.cpp -o test  && ./test