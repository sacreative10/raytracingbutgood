//
// Created by sohaibalam on 27/05/24.
//

#include "perlin.h"
float perlin::noise(const point3 &p) const {

  auto u = p.x - floor(p.x);
  auto v = p.y - floor(p.y);
  auto w = p.z - floor(p.z);

  int i = floor(p.x);
  int j = floor(p.y);
  int k = floor(p.z);

  Vec3 c[2][2][2];

  for (int di = 0; di < 2; di++)
    for (int dj = 0; dj < 2; dj++)
      for (int dk = 0; dk < 2; dk++)
        c[di][dj][dk] =
            randvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^
                    perm_z[(k + dk) & 255]];

  return perlin_interp(c, u, v, w);
}
float perlin::perlin_interp(const Vec3 (*c)[2][2], float u, float v, float z) {
  auto uu = u * u * (3 - 2 * u);
  auto vv = v * v * (3 - 2 * v);
  auto ww = z * z * (3 - 2 * z);
  auto accum = 0.0;

  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < 2; k++) {
        Vec3 weight_v(u - i, v - j, z - k);
        accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) *
                 (k * ww + (1 - k) * (1 - ww)) * glm::dot(c[i][j][k], weight_v);
      }

  return accum;
}
float perlin::turb(const point3 &p, int depth) const {
  auto accum = 0.0f;
  auto temp_p = p;
  auto weight = 1.0f;

  for (int i = 0; i < depth; i++) {
    accum += weight * noise(temp_p);
    weight *= 0.5;
    temp_p *= 2;
  }

  return fabs(accum);
}
