#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <utility>

#include "Ray.h"
#include "hittable.h"
#include "quadilateral.h"

class Triangle : public quadilateral {
public:
  Triangle(const point3 &o, const Vec3 &ab, const Vec3 &ac,
           shared_ptr<material> mat)
      : quadilateral(o, ab, ac, std::move(mat)) {}

  virtual bool is_interior(float a, float b, hitrecord &rec) const override {
    if ((a < 0) || (b < 0) || (a + b > 1))
      return false;

    rec.uv = Vec2(a, b);
    return true;
  }
};

#endif
