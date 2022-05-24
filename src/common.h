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
#include "Ray.h"
#include <glm/glm.hpp>


#endif
