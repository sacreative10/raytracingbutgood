
#include "texture.h"
#include <cmath>

color checkered::value(point2 uv, const point3 &p) const {
  int xInt = int(std::floor(inv_scale * p.x));
  int yInt = int(std::floor(inv_scale * p.y));
  int zInt = int(std::floor(inv_scale * p.z));

  bool isEven = (xInt + yInt + zInt) & 1;

  return isEven ? even->value(uv, p) : odd->value(uv, p);
}

