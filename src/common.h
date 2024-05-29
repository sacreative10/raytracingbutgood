#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <limits>
#include <memory>

// some usings to make life easier
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

using Vec3 = glm::vec3;
using point3 = glm::vec3;
using color = glm::vec3;
using point2 = glm::vec2;

// some constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.14159265358979323846;

// utility functions

inline float deg_to_reg(float deg) { return deg * pi / 180; }

// common includes

#include <random>

inline float random_float() {
  static std::uniform_real_distribution<float> distribution(0.0, 1.0);
  static std::mt19937 generator;
  return distribution(generator);
}

inline float random_float(float min, float max) {
  return min + (max - min) * random_float();
}

inline Vec3 randomVector() {
  return Vec3(random_float(), random_float(), random_float());
}

inline Vec3 randomVector(float min, float max) {
  return Vec3(random_float(min, max), random_float(min, max),
              random_float(min, max));
}

inline int random_int(int min, int max) {
  return int(random_float(min, max + 1));
}

inline float clamp(float x, float min, float max) {
  if (x < min)
    return min;
  if (x > max)
    return max;
  return x;
}

inline int clamp(int x, int min, int max) {
  if (x < min)
    return min;
  if (x > max)
    return max;
  return x;
}

#include "Ray.h"
#include <glm/glm.hpp>

#endif
