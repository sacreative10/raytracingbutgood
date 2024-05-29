//
// Created by sohaibalam on 26/05/24.
//

#ifndef RAYTRACINGBUTGOOD_AABB_H
#define RAYTRACINGBUTGOOD_AABB_H

#include "Ray.h"
#include "geometry.h"
#include "interval.h"
class aabb {
public:
  interval x, y, z;

  aabb(const interval &x, const interval &y, const interval &z)
      : x(x), y(y), z(z) {
    pad_to_minimums();
  }

  aabb(const Point3 &a, const Point3 &b) {
    x = a[0] < b[0] ? interval(a[0], b[0]) : interval(b[0], a[0]);
    y = a[1] < b[1] ? interval(a[1], b[1]) : interval(b[1], a[1]);
    z = a[2] < b[2] ? interval(a[2], b[2]) : interval(b[2], a[2]);
  }

  aabb(const aabb &box0, const aabb &box1) {
    x = interval(box0.x, box1.x);
    y = interval(box0.y, box1.y);
    z = interval(box0.z, box1.z);
  }

  const interval &axis_interval(int n) const {
    if (n == 0)
      return x;
    if (n == 1)
      return y;
    return z;
  }

  int longest_axis() const {
    if (x.size() > y.size())
      return x.size() > z.size() ? 0 : 2;
    return y.size() > z.size() ? 1 : 2;
  }
  bool hit(const Ray &r, interval ray_t) const;

public:
  aabb() {}
  static const aabb empty, universe;

private:
  // to avoid division by zero
  void pad_to_minimums() {
    float delta = 0.0001;
    if (x.size() < delta)
      x.expands(delta);
    if (y.size() < delta)
      y.expands(delta);
    if (z.size() < delta)
      z.expands(delta);
  }
};

aabb operator+(const aabb &bbox, const Vec3 &offset);

#endif // RAYTRACINGBUTGOOD_AABB_H
