
#ifndef RAYTRACINGBUTGOOD_TEXTURE_H
#define RAYTRACINGBUTGOOD_TEXTURE_H
#include "common.h"
#include "geometry.h"
#include <memory>

class Texture {
public:
  virtual ~Texture() = default;

  virtual color value(point2 uv, const point3 &p) const = 0;
};

class solid_colour : public Texture {
public:
  solid_colour(const color &albedo) : albedo(albedo) {}

  color value(point2 uv, const point3 &p) const override { return albedo; }

private:
  color albedo;
};

class checkered : public Texture {
public:
  checkered(float scale, std::shared_ptr<Texture> even,
            std::shared_ptr<Texture> odd)
      : inv_scale(1.0f / scale), even(even), odd(odd) {}

  checkered(float scale, const color &c1, const color &c2)
      : inv_scale(1.0f / scale), even(make_shared<solid_colour>(c1)),
        odd(make_shared<solid_colour>(c2)) {}

  color value(point2 uv, const point3 &p) const override;

private:
  float inv_scale;
  shared_ptr<Texture> even;
  shared_ptr<Texture> odd;
};

#endif // ifndef!RAYTRACINGBUTGOOD_TEXTURE_H
