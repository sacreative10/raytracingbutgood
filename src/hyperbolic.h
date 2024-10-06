//
// Created by sohaibalam on 05/10/24.
//

#ifndef HYPERBOLIC_H
#define HYPERBOLIC_H

#include "texture.h"

namespace poincareDisk
{
  float hyperbolicDistanceFromOrigin(const point3 &p);
  float hyperbolicDistance(const point3& p1, const point3& p2);
  float angularCoordinate(const point3& p);
  bool inRegion(const point3 &p, float scale);
};


class hyperbolic_texture : public Texture
{
public:
  hyperbolic_texture(float scale, std::shared_ptr<Texture> texA, std::shared_ptr<Texture> texB)
      : scale(scale), texA(texA), texB(texB) {}

  hyperbolic_texture(float scale, const color &c1, const color &c2)
      : scale(1.0f / scale), texA(make_shared<solid_colour>(c1)),
        texB(make_shared<solid_colour>(c2)) {}

  color value(point2 uv, const point3 &p) const override;

private:
  float scale;
  shared_ptr<Texture> texA;
  shared_ptr<Texture> texB;
};



#endif //HYPERBOLIC_H
