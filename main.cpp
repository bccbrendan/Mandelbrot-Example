#include "Mandelbrot.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){

    const string NUM_THREADS = argv[0];
    std::cout << "Number of Threads = " << NUM_THREADS;


    Mandelbrot::Mandelbrot myplot(1920, 1080);
    myplot.draw("./images/green-1-thread.bmp", Mandelbrot::Mandelbrot::GREEN);

    return 0;
}