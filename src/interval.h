//
// Created by sohaibalam on 26/05/24.
//

#ifndef RAYTRACINGBUTGOOD_INTERVAL_H
#define RAYTRACINGBUTGOOD_INTERVAL_H

#include "common.h"

class interval {
public:
  double min, max;

  interval() : min(-infinity), max(infinity) {}
  interval(double min, double max) : min(min), max(max) {}
  float size() const { return max - min; }
  bool contains(double x) const { return x >= min && x <= max; }
  bool surrounds(const interval &other) const {
    return min <= other.min && max >= other.max;
  }
  interval expands(double delta) {
    auto padding = delta / 2;
    return interval(min - padding, max + padding);
  }
  float _clamp(float x) { return clamp(x, min, max); }
  interval(const interval &a, const interval &b)
      : min(std::min(a.min, b.min)), max(std::max(a.max, b.max)) {}

  static const interval empty, universe;
};

#endif // RAYTRACINGBUTGOOD_INTERVAL_H
