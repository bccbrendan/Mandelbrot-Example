#include "Mandelbrot.h"

using namespace std;

int main(){


    Mandelbrot::Mandelbrot myplot(1920, 1080);
    myplot.draw("./images/green-1-thread.bmp", Mandelbrot::Mandelbrot::GREEN);

    return 0;
}