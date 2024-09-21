// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2022 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR
// LicenseRef-KDE-Accepted-GPL

#include "sphere.h"

Sphere::Sphere(point3 cen, float r, shared_ptr<material> mat)
    : m_center(cen), m_radius(r), m_mat(mat) {
  auto rvec = glm::vec3(r, r, r);
  bbox = aabb(m_center - rvec, m_center + rvec);
}

// this functon uses a hitrecord and updates it according to whether the this
// specifc object has been hit or not
bool Sphere::hit(const Ray &r, float t_min, float t_max, hitrecord &rec) const {
  glm::vec3 oc = r.origin() - m_center;
  float a = squareVector(r.direction());
  float half_b = glm::dot(oc, r.direction());
  float c = squareVector(oc) - m_radius * m_radius;

  // find the discriminant
  auto discriminant = half_b * half_b - a * c;
  if (discriminant < 0)
    return false;
  float sqrtd = sqrtf(discriminant);

  // find the nearest root in the range given
  float root = (-half_b - sqrtd) / a;
  if (root < t_min || t_max < root) {
    root = (-half_b + sqrtd) / a;
    if (root < t_min || t_max < root)
      return false;
  }

  rec.t = root;
  rec.p = r.at(rec.t);
  glm::vec3 outward_normal = (rec.p - m_center) / m_radius;
  rec.set_face_normal(r, outward_normal);
  rec.uv = get_sphere_uv(outward_normal);
  rec.mat_ptr = m_mat;

  return true;
}
