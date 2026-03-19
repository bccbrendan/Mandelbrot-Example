#include <complex>
#include <iostream>
#include <vector>
#include <math.h>
#include "Mandelbrot.h"

using namespace std;

namespace Mandelbrot{

Mandelbrot::Mandelbrot(int width, int height, const int N_THREADS):
    _bitmap(width, height),
    _width(width),
    _height(height), 
    colourHistogram(new int[MAX_ITERATIONS]{0}),
    fractalData(new int[_width*_height]),
    NUM_THREADS(N_THREADS)
    {
}

void Mandelbrot::draw(string fileName, drawColor colourSelection ){

    int* p = colourHistogram.get();
    int* pfractalData = fractalData.get();

    
    for (int y = 0; y < _height; y+= 1){
        for (int x = 0; x < _width; x++){
            double xFractal = (x - _width/2 - 150) * 2.0/_width;
            double yFractal = (y - _height/2) * 2.0/_width;

            int num_iters = getIterations(xFractal, yFractal);
                
            // create a pixel map for data
            pfractalData[y*_width + x] = num_iters;

            // remove max interations from histogram for prettier plot.
            if (num_iters != MAX_ITERATIONS){
                p[num_iters]++;
            }
                
        }
    }

    int total{0};
    for (int i =0; i < MAX_ITERATIONS; i++){
        total += colourHistogram[i];
    }
    
    for (int y = 0; y < _height; y++){
        for (int x = 0; x < _width; x++){

            uint8_t red = 0;
            uint8_t green = 0;
            uint8_t blue = 0;

            int num_iters = pfractalData[y*_width + x];
            if (num_iters != MAX_ITERATIONS){
                double hue{0.0};
                for(int i = 0; i < num_iters; i++){
                    hue += (double)colourHistogram[i] / total; // divide by total num of pixels
                }

                // uint8_t colour = (uint8_t) (255 * (double)num_iters / MAX_ITERATIONS);

                //colour = colour*colour*colour; // make small colours bigger

                red = pow(255, hue);
                green = pow(255, hue);
                blue = pow(255, hue);
            }
            // Adjust hue

            switch(colourSelection){
                case RED:
                    _bitmap.setPixel(x, y, red, 0, 0);
                    break;
                case GREEN:
                    _bitmap.setPixel(x, y, 0, green, 0);
                    break;
                case BLUE:
                    _bitmap.setPixel(x, y, 0, 0, blue);
                    break;

            }

        }
    }
    _bitmap.write(fileName);
    return;
}

int Mandelbrot::getIterations(double x, double y){
    /*
    Will return any number up to MAX_ITERATIONS (1024)
    */
    const int THRESHOLD = 2; // 2
    
    complex<double> z(0); // 0 + 0j
    complex<double> c(x,y); // x + yj

    int iterations{0};

    while (iterations < MAX_ITERATIONS){
        z = (z*z) + c;
        if (abs(z) > THRESHOLD){
            break;
        }
        iterations++;
    }
    return iterations;
}


void Mandelbrot::printHistogram(){
    int* p = colourHistogram.get();

    for(int i = 0; i < MAX_ITERATIONS; i++){
        std::cout << p[i] << std::endl;
    }

}

bool Mandelbrot::_validHistogram(){
    /*
    Sum counts in histogram bins and check if equal to number of pixels
    */
    int* p = colourHistogram.get();
    int numPixels = _width*_height;
    int count{0};

    for(int i = 0; i < MAX_ITERATIONS; i++){
        count += p[i];
    }

    if (count == numPixels){
        return true;
    }

    return false;
}
}

