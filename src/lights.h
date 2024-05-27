//
// Created by sohaibalam on 27/05/24.
//

#ifndef RAYTRACINGBUTGOOD_LIGHTS_H
#define RAYTRACINGBUTGOOD_LIGHTS_H

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

#endif // RAYTRACINGBUTGOOD_LIGHTS_H
