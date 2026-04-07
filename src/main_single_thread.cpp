#include "Mandelbrot.h"
#include <iostream>
#include <limits.h>
#include <string>
#include <unistd.h>

using namespace std;

namespace {
std::string images_dir_path() {
    char executable_path[PATH_MAX];
    const ssize_t path_length = readlink("/proc/self/exe", executable_path, sizeof(executable_path) - 1);

    if (path_length == -1) {
        return "images";
    }

    executable_path[path_length] = '\0';
    std::string executable_dir(executable_path);
    const std::size_t last_separator = executable_dir.find_last_of('/');

    if (last_separator == std::string::npos) {
        return "images";
    }

    executable_dir.erase(last_separator);
    return executable_dir + "/../images";
}
}

int main(int argc, char* argv[]){

    const int NUM_THREADS = 1;
    std::cout << "Number of Threads = " << NUM_THREADS << std::endl;

    Mandelbrot::Mandelbrot myplot(1920, 1080, NUM_THREADS);
    myplot.draw(images_dir_path() + "/Green-Single-Thread-512.bmp", Mandelbrot::Mandelbrot::GREEN);

    return 0;
}
