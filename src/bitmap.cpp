#include <fstream>
#include <iostream>
#include <cmath>
#include "bitmap.h"
#include "BitmapFileHeader.h"
#include "BitmapInfoHeader.h"

using namespace Mandelbrot;
using namespace std;

namespace Mandelbrot{

Bitmap::Bitmap(int width, int height):_width(width),_height(height),_pPixels(new uint8_t[height*width*3]{0}){

}

bool Bitmap::write(string fileName){
    BitmapFileHeader fileHeader;
    BitmapInfoHeader infoHeader;

    fileHeader.filesize = (sizeof(BitmapFileHeader)+sizeof(BitmapInfoHeader)+(_width*_height*3));
    fileHeader.dataOffset = (sizeof(BitmapFileHeader)+sizeof(BitmapInfoHeader));

    infoHeader.width = _width;
    infoHeader.height = _height;

    std::ofstream file;

    file.open(fileName, std::ios::binary);

    if(!file){
        return false;
    }
    // Actually write bitmap to file
    file.write((char*)&fileHeader, sizeof(fileHeader));
    file.write((char*)&infoHeader, sizeof(infoHeader));
    file.write((char*)_pPixels.get(), _width*_height*3);

    file.close();

    if (!file){
        return false;
    }

    return true;

}

void Bitmap::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b){
    // update value pointed to by _pPixel
    uint8_t *pPixel = _pPixels.get();
    pPixel += 3*(y*_width + x); // update pointer location, i.e. +=

    *(pPixel + 0) = b; // equivalent to *(pPixel + 0)
    pPixel[1] = g;
    pPixel[2] = r;
    
}

void Bitmap::fillColour(uint8_t r, uint8_t g, uint8_t b){
    for (int y = 0; y < _height; y++){
        for(int x = 0; x < _width; x++){
            this->setPixel(x,y,r,g,b);
        }
    }
}

void Bitmap::getScaleFactorAndOffset(){
    // Find smallest and biggest number in bitmap
    uint8_t current_min{static_cast<uint8_t>(0)};
    uint8_t current_max{static_cast<uint8_t>(0)};
    uint8_t* pPixel = _pPixels.get();
    int numBytes = _width * _height * 3;

    for (int y = 0; y < numBytes; y++, pPixel++){
        if (*pPixel < current_min) current_min = *pPixel;
        if (*pPixel > current_max) current_max = *pPixel;
        
    }
    std::cout << "Current Max =" << static_cast<int>(current_max) << std::endl;
    std::cout << "Current Min =" << static_cast<int>(current_min) << std::endl;

    double MAX_VALUE{1.0};
    double MIN_VALUE{-1.0};

    double scale_factor = (double)(current_max - current_min) / (MAX_VALUE - MIN_VALUE);
    double offset = MIN_VALUE - (round((double)current_min / scale_factor));
    offset = min(MIN_VALUE, offset);

    std::cout << "Scale factor = " << scale_factor << std::endl;
    
    std::cout << "Offset = " << offset << std::endl;
}

Bitmap::~Bitmap(){

}

}