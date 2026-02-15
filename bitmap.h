#pragma once
#include <string>
#include <memory>
#include <cstdint>

using namespace std;

namespace Mandelbrot{

class Bitmap{
private:
    int _width{0};
    int _height{0};
    double _scale_factor;
    int _offset;
    unique_ptr<uint8_t[]> _pPixels{nullptr};
public:
    Bitmap(int width, int height);
    bool write(string fileName);
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void fillColour(uint8_t r, uint8_t g, uint8_t b);
    void getScaleFactorAndOffset();
    virtual ~Bitmap();    
};

}