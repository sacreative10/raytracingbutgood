//
// Created by sohaibalam on 27/05/24.
//

#ifndef RAYTRACINGBUTGOOD_LIGHTS_H
#define RAYTRACINGBUTGOOD_LIGHTS_H

#include "glm/gtx/norm.hpp"
#include "material.h"
class diffuseLights : public material {
public:
  diffuseLights(shared_ptr<Texture> tex) : tex(tex) {}
  diffuseLights(const color &emittedColor)
      : tex(make_shared<solid_colour>(emittedColor)) {}

  color emitted(const point2 &uv, const point3 &p) const override {
    return tex->value(uv, p);
  }

private:
  shared_ptr<Texture> tex;
};

class spotLight : public material {
public:
  spotLight(const color &emittedColor, float totalWidth, float falloffStart)
      : emittedColor(emittedColor) {
    cosTotalWidth = std::cos(deg_to_reg(totalWidth));
    cosFalloffStart = std::cos(deg_to_reg(falloffStart));
  }

  color emitted(const point2 &uv, const point3 &p) const override {
    return emittedColor * falloff(glm::normalize(p));
  }

  float falloff(const Vec3 &w) const {
    Vec3 wl = glm::normalize(w);
    float cosTheta = wl.z;
    if (cosTheta < cosTotalWidth)
      return 0;
    if (cosTheta > cosFalloffStart)
      return 1;
    // Compute falloff inside spotlight cone
    float delta =
        (cosTheta - cosTotalWidth) / (cosFalloffStart - cosTotalWidth);
    return delta * delta * delta * delta;
  }

private:
  color emittedColor;
  float cosTotalWidth, cosFalloffStart;
};

#endif // RAYTRACINGBUTGOOD_LIGHTS_H
