// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2022 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR
// LicenseRef-KDE-Accepted-GPL

#ifndef HITTABLE_H
#define HITTABLE_H

#include "common.h"

#include "Ray.h"
#include "aabb.h"
/**
 * @todo write docs
 */
class material;

struct hitrecord {
  point3 p;
  glm::vec3 normal;
  float t;
  shared_ptr<material> mat_ptr;
  point2 uv;
  bool front_face;

  inline void set_face_normal(const Ray &r, const glm::vec3 &outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class Hittable {
public:
  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hitrecord &rec) const = 0;

  virtual aabb bounding_box() const = 0;
};

#endif // HITTABLE_H
