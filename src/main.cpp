#include "Mandelbrot.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){

    const int NUM_THREADS = std::stoi(argv[1]);
    std::cout << "Number of Threads = " << NUM_THREADS << std::endl;

    Mandelbrot::Mandelbrot myplot(1920, 1080, NUM_THREADS);
    myplot.draw("/home/ec2-user/Mandelbrot-final/Mandelbrot-Example/images/Green-Parallel-512.bmp", Mandelbrot::Mandelbrot::GREEN);

    return 0;
}