#include <complex>
#include <thread>
#include <iostream>
#include <vector>
#include <math.h>
#include <cstdint>
#if defined(__has_include)
#if __has_include(<arm_sve.h>)
#define MANDELBROT_HAS_ARM_SVE_HEADER 1
#include <arm_sve.h>
#endif
#if __has_include(<arm_sve2.h>)
#define MANDELBROT_HAS_ARM_SVE2_HEADER 1
#include <arm_sve2.h>
#endif
#endif
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include "Mandelbrot.h"

#if defined(MANDELBROT_USE_SVE2) && !defined(MANDELBROT_USE_SVE)
#define MANDELBROT_USE_SVE 1
#endif

#if defined(MANDELBROT_USE_SVE)
#if !defined(MANDELBROT_HAS_ARM_SVE_HEADER)
#error "MANDELBROT_USE_SVE requires <arm_sve.h>."
#endif
#if !defined(__ARM_FEATURE_SVE)
#error "MANDELBROT_USE_SVE requires an SVE target (e.g. -march=armv8.2-a+sve)."
#endif
#endif

#if defined(MANDELBROT_USE_SVE2)
#if !defined(MANDELBROT_HAS_ARM_SVE2_HEADER)
#error "MANDELBROT_USE_SVE2 requires <arm_sve2.h>."
#endif
#if !defined(__ARM_FEATURE_SVE2)
#error "MANDELBROT_USE_SVE2 requires an SVE2 target (e.g. -march=armv9-a+sve2)."
#endif
#endif


using namespace std;

namespace Mandelbrot{

#if defined(MANDELBROT_USE_SVE)
static inline void getIterationsSve(svfloat32_t c_re, svfloat32_t c_im, int32_t out_iters[], svbool_t pg){
    const svfloat32_t threshold2 = svdup_f32(4.0f);
    svfloat32_t z_re = svdup_f32(0.0f);
    svfloat32_t z_im = svdup_f32(0.0f);
    svint32_t iters = svdup_s32(0);
    svbool_t active = pg;

    for (int i = 0; i < Mandelbrot::MAX_ITERATIONS; i++){
        if (!svptest_any(pg, active)){
            break;
        }

        svfloat32_t z_re2 = svmul_f32_x(pg, z_re, z_re);
        svfloat32_t z_im2 = svmul_f32_x(pg, z_im, z_im);
        svfloat32_t z_re_im = svmul_f32_x(pg, z_re, z_im);

        svfloat32_t z_re_new = svadd_f32_x(pg, svsub_f32_x(pg, z_re2, z_im2), c_re);
        svfloat32_t z_im_new = svadd_f32_x(pg, svadd_f32_x(pg, z_re_im, z_re_im), c_im);

        svfloat32_t mag2 = svadd_f32_x(pg, svmul_f32_x(pg, z_re_new, z_re_new), svmul_f32_x(pg, z_im_new, z_im_new));
        svbool_t still_in = svcmple_f32(pg, mag2, threshold2);

        svbool_t inc_mask = svand_b_z(pg, active, still_in);
        iters = svadd_s32_m(inc_mask, iters, svdup_s32(1));

        z_re = svsel_f32(inc_mask, z_re_new, z_re);
        z_im = svsel_f32(inc_mask, z_im_new, z_im);
        active = inc_mask;
    }

    svst1_s32(pg, out_iters, iters);
}
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
static inline void getIterationsNeon4(float32x4_t c_re, float32x4_t c_im, int out_iters[4]){
    const float32x4_t threshold2 = vdupq_n_f32(4.0f);
    float32x4_t z_re = vdupq_n_f32(0.0f);
    float32x4_t z_im = vdupq_n_f32(0.0f);
    int32x4_t iters = vdupq_n_s32(0);
    uint32x4_t active = vdupq_n_u32(0xFFFFFFFFu);

    for (int i = 0; i < Mandelbrot::MAX_ITERATIONS; i++){
        float32x4_t z_re2 = vmulq_f32(z_re, z_re);
        float32x4_t z_im2 = vmulq_f32(z_im, z_im);
        float32x4_t z_re_im = vmulq_f32(z_re, z_im);

        float32x4_t z_re_new = vaddq_f32(vsubq_f32(z_re2, z_im2), c_re);
        float32x4_t z_im_new = vaddq_f32(vaddq_f32(z_re_im, z_re_im), c_im);

        float32x4_t mag2 = vaddq_f32(vmulq_f32(z_re_new, z_re_new), vmulq_f32(z_im_new, z_im_new));
        uint32x4_t still_in = vcleq_f32(mag2, threshold2);

        uint32x4_t inc_mask = vandq_u32(active, still_in);
        iters = vaddq_s32(iters, vreinterpretq_s32_u32(vandq_u32(inc_mask, vdupq_n_u32(1))));

        z_re = z_re_new;
        z_im = z_im_new;
        active = inc_mask;

        if (vmaxvq_u32(active) == 0){
            break;
        }
    }

    vst1q_s32(out_iters, iters);
}
#endif

Mandelbrot::Mandelbrot(int width, int height, const int N_THREADS):
    _bitmap(width, height),
    _width(width),
    _height(height), 
    colourHistogram(new int[MAX_ITERATIONS]{0}),
    fractalData(new int[_width*_height]),
    NUM_THREADS(N_THREADS)
    {
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

void Mandelbrot::draw(string fileName, drawColor colourSelection ){

    int* p = colourHistogram.get();
    int* pfractalData = fractalData.get();

    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS);

    auto work = [&](int thread_id){
#if defined(MANDELBROT_USE_SVE)
        const uint64_t sve_lanes = svcntw();
        std::vector<int32_t> sve_iters(sve_lanes);
#endif
        for (int y = thread_id; y < _height; y+= NUM_THREADS){
            int x = 0;
#if defined(MANDELBROT_USE_SVE)
            const float x_step = 2.0f/_width;
            const float x_base = (-_width/2.0f - 150.0f) * x_step;
            const float y_fractal = (y - _height/2.0f) * x_step;
            for (; x < _width; x += static_cast<int>(sve_lanes)){
                svbool_t pg = svwhilelt_b32(static_cast<uint64_t>(x), static_cast<uint64_t>(_width));
                svint32_t x_vals = svindex_s32(x, 1);
                svfloat32_t x_offsets = svcvt_f32_s32_x(pg, x_vals);
                svfloat32_t c_re = svadd_f32_x(pg, svdup_f32(x_base), svmul_n_f32_x(pg, x_offsets, x_step));
                svfloat32_t c_im = svdup_f32(y_fractal);

                getIterationsSve(c_re, c_im, sve_iters.data(), pg);

                unique_lock<mutex> l(histMutex);
                const int active_lanes = static_cast<int>(svcntp_b32(svptrue_b32(), pg));
                for (int lane = 0; lane < active_lanes; lane++){
                    int idx = y*_width + (x + lane);
                    int num_iters = sve_iters[lane];
                    pfractalData[idx] = num_iters;
                    if (num_iters != MAX_ITERATIONS){
                        p[num_iters]++;
                    }
                }
                l.unlock();
            }
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
            const float32x4_t x_step = vdupq_n_f32(2.0f/_width);
            const float32x4_t x_base = vdupq_n_f32((-_width/2.0f - 150.0f) * 2.0f/_width);
            const float32x4_t y_fractal = vdupq_n_f32((y - _height/2.0f) * 2.0f/_width);
            for (; x + 3 < _width; x += 4){
                int32_t x_vals[4] = {x, x + 1, x + 2, x + 3};
                float32x4_t x_offsets = vcvtq_f32_s32(vld1q_s32(x_vals));
                float32x4_t c_re = vaddq_f32(x_base, vmulq_f32(x_offsets, x_step));
                float32x4_t c_im = y_fractal;

                int iters[4];
                getIterationsNeon4(c_re, c_im, iters);

                unique_lock<mutex> l(histMutex);
                for (int lane = 0; lane < 4; lane++){
                    int idx = y*_width + (x + lane);
                    int num_iters = iters[lane];
                    pfractalData[idx] = num_iters;
                    if (num_iters != MAX_ITERATIONS){
                        p[num_iters]++;
                    }
                }
                l.unlock();
            }
#endif
            for (; x < _width; x++){
                double xFractal = (x - _width/2 - 150) * 2.0/_width;
                double yFractal = (y - _height/2) * 2.0/_width;

                int num_iters = getIterations(xFractal, yFractal);
                
                unique_lock<mutex> l(histMutex);
                // create a pixel map for data
                pfractalData[y*_width + x] = num_iters;

                // remove max interations from histogram for prettier plot.
                if (num_iters != MAX_ITERATIONS){
                    p[num_iters]++;
                }
                l.unlock();
            }
        }
    };

    for(int tid = 0; tid < NUM_THREADS; tid++){
        threads.emplace_back(work, tid);
    }

    for(auto& i: threads){
        i.join();
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
