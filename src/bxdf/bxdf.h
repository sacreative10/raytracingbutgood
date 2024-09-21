//
// Created by sohaibalam on 31/05/24.
//

#ifndef RAYTRACINGBUTGOOD_BXDF_H
#define RAYTRACINGBUTGOOD_BXDF_H

#include "common.h"
enum BxDFType {
  BSDF_REFLECTION = 1 << 0,
  BSDF_TRANSMISSION = 1 << 1,
  BSDF_DIFFUSE = 1 << 2,
  BSDF_GLOSSY = 1 << 3,
  BSDF_SPECULAR = 1 << 4,
  BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION |
             BSDF_TRANSMISSION
};

class BxDF {
public:
  BxDF(BxDFType type) : type(type) {}
  virtual ~BxDF() = default;

  // Evaluate the BxDF for the given pair of directions
  virtual color f(const Vec3 &wo, const Vec3 &wi) const = 0;

  // Sample the BxDF for the given direction used for perfect specular
  virtual color sample_f(const Vec3 &wo, Vec3 &wi, float &pdf,
                         const Vec2 &sample) const = 0;

  bool matches(BxDFType t) const { return (type & t) == type; }

  BxDFType type;
};

#endif // RAYTRACINGBUTGOOD_BXDF_H
