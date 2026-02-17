#pragma once
#include "bitmap.h"
#include <string>
#include <condition_variable>
#include <mutex>
#include <memory>

namespace Mandelbrot{

class Mandelbrot{
public:
    Bitmap _bitmap;
    int _width{0};
    int _height{0};
    enum drawColor{RED,GREEN,BLUE};
    unique_ptr<int[]> colourHistogram;
    unique_ptr<int[]> fractalData;

public:
    Mandelbrot(int width, int height, const int N_THREADS);
    static int getIterations(double x, double y);
    static const int MAX_ITERATIONS = (1<<9);
    void draw(string fileName, drawColor color);
    void printHistogram();
    ~Mandelbrot(){};
private:
    bool _validHistogram();
    mutex histMutex;
    condition_variable _histcv;
    const int NUM_THREADS{1};

};

};