//
// Created by sohaibalam on 26/05/24.
//

#include "aabb.h"

const aabb aabb::empty =
    aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe =
    aabb(interval::universe, interval::universe, interval::universe);

bool aabb::hit(const Ray &r, interval ray_t) const {
  const point3 &origin = r.origin();
  const Vec3 &direction = r.direction();

  for (int axis = 0; axis < 3; axis++) {
    const interval &ax = axis_interval(axis);
    const double adinv = 1.0 / direction[axis];

    double t0 = (ax.min - origin[axis]) * adinv;
    double t1 = (ax.max - origin[axis]) * adinv;

    if (t0 < t1) {
      if (t0 > ray_t.min)
        ray_t.min = t0;
      if (t1 < ray_t.max)
        ray_t.max = t1;
    } else {
      if (t1 > ray_t.min)
        ray_t.min = t1;
      if (t0 < ray_t.max)
        ray_t.max = t0;
    }

    if (ray_t.max <= ray_t.min)
      return false;
  }
  return true;
}
