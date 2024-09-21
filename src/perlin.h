//
// Created by sohaibalam on 27/05/24.
//

#ifndef RAYTRACINGBUTGOOD_PERLIN_H
#define RAYTRACINGBUTGOOD_PERLIN_H
#include <cmath>

#include "common.h"
#include "texture.h"

class perlin {
public:
  perlin() {
    randvec = new Vec3[point_count];
    for (int i = 0; i < point_count; i++)
      randvec[i] = glm::normalize(randomVector(-1, 1));

    perm_x = perlin_generate_perm();
    perm_y = perlin_generate_perm();
    perm_z = perlin_generate_perm();
  }

  ~perlin() {
    delete[] randvec;
    delete[] perm_x;
    delete[] perm_y;
    delete[] perm_z;
  }

  float noise(const point3 &p) const;

  float turb(const point3 &p, int depth) const;

private:
  static int *perlin_generate_perm() {
    auto p = new int[point_count];
    for (int i = 0; i < point_count; i++)
      p[i] = i;
    permute(p, point_count);
    return p;
  }

  static void permute(int *p, int n) {
    for (int i = n - 1; i > 0; i--) {
      int target = random_int(0, i);
      int tmp = p[i];
      p[i] = p[target];
      p[target] = tmp;
    }
  }

  static float perlin_interp(const Vec3 c[2][2][2], float u, float v, float z);
  static const int point_count = 256;
  Vec3 *randvec;
  int *perm_x;
  int *perm_y;
  int *perm_z;
};

class perlin_noise_texture : public Texture {
public:
  perlin_noise_texture() {}

  perlin_noise_texture(float sc) : scale(sc) {}

  color value(point2 uv, const point3 &p) const override {
    return color(0.5) *
           (float)(1.0f + std::sin(scale * p.z + 10 * noise.turb(p, 7)));
  }

private:
  perlin noise;
  float scale;
};

#endif // RAYTRACINGBUTGOOD_PERLIN_H
