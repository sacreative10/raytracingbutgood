// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2022 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR
// LicenseRef-KDE-Accepted-GPL

#ifndef SPHERE_H
#define SPHERE_H

#include "common.h"
#include "hittable.h"
#include <cmath>

/**
 * @todo write docs
 */
class Sphere : public Hittable {
public:
  Sphere() {}
  Sphere(point3 cen, float r, shared_ptr<material> mat);

  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hitrecord &rec) const override;

  aabb bounding_box() const override { return bbox; }

  static point2 get_sphere_uv(const point3 &p) {
    auto theta = acos(-p.y);
    auto phi = atan2(-p.z, p.x) + pi;

    return {phi / (2 * pi), theta / pi};
  }

public:
  point3 m_center;
  float m_radius;
  shared_ptr<material> m_mat;

  aabb bbox;
};

#endif // SPHERE_H
