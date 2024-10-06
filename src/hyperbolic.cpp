//
// Created by sohaibalam on 05/10/24.
//

#include "hyperbolic.h"

namespace poincareDisk
{
  float hyperbolicDistanceFromOrigin(const point3& p)
  {
    float r = p.length();

    if (r >= 1.0f)
      return std::numeric_limits<float>::infinity();

    return std::log((1.f + r) / (1.f - r));
  }

  float hyperbolicDistance(const point3& p1, const point3& p2)
  {
    float d1 = hyperbolicDistanceFromOrigin(p1);
    float d2 = hyperbolicDistanceFromOrigin(p2);

    if (d1 >= 1.f || d2 >= 1.f)
      return std::numeric_limits<float>::infinity();

    float numerator = 2.f * std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
    float denominator = (1.f - d1 * d1) * (1.f - d2 * d2);
    return std::acosh(numerator / denominator);
  }
  float angularCoordinate(const point3& p)
  {
    return std::atan2(p.y, p.x);
  }

  bool inRegion(const point3& p, float scale)
  {
    float dist = hyperbolicDistanceFromOrigin(p);
    float angle = angularCoordinate(p);

    if (angle < 0)
      angle += 2 * M_PI;

    float numAngularDivisions = 8;
    float angularRegionSize = (2.f * M_PI) / numAngularDivisions;
    int region = int(angle / angularRegionSize);

    return (fmod(dist * scale, 2.f) < 1.f) && (region % 2 == 0);
  }
};


color hyperbolic_texture::value(point2 uv, const point3& p) const
{
  float dist = p.length();
  float angle = std::atan2(p.y, p.x);

  if (angle < 0)
    angle += 2 * M_PI;

  int numSectors = 12;
  int rings = 10;

  float sectorAngle = 2 * M_PI / numSectors;

  int sectorindex = int(angle / sectorAngle);
  int ringindex = int(dist * rings);

  if (ringindex + sectorindex % 2 == 0)
    return texA->value(uv, p);
  else
    return texB->value(uv, p);

}