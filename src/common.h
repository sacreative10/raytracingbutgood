#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include <limits>
#include <memory>


// some usings to make life easier
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// some constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.14159265358979323846;

// utility functions

inline float deg_to_reg(float deg)
{
    return deg * pi / 180;
}

// common includes


#include <random>


inline float random_float()
{
    static std::uniform_real_distribution<float> distribuiton(0.0f, 1.0f);
    static std::mt19937 generator;
    return distribuiton(generator);
}

inline float random_float(float min, float max)
{
    static std::uniform_real_distribution<float> distribuiton(min, max);
    static std::mt19937 generator;
    return distribuiton(generator);
}

inline float clamp(float x, float min, float max)
{
    if( x < min ) return min;
    if( x > max ) return max;
    return x;
}


#include "Ray.h"
#include <glm/glm.hpp>


#endif
