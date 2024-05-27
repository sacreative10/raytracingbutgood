//
// Created by sohaibalam on 27/05/24.
//

#include "quadilateral.h"
bool quadilateral::hit(const Ray &r, float t_min, float t_max,
                       hitrecord &rec) const {
  // the general equation for a plane can be written as Ax + By + Cz = D
  // Which can then be generalized as n.p = D where n is the normal vector and p
  // is a point on the plane
  // The ray equation is p(t) = A + tb
  // Substituting the ray equation into the plane equation we get
  // n.(A + tb) = D => n.A + t(n.b) = D
  // t = (D - n.A) / n.b

  auto denominator = glm::dot(normal, r.direction());
  if (std::abs(denominator) < 1e-6)
    return false;

  auto t = (D - glm::dot(normal, r.origin())) / denominator;
  if (t < t_min || t > t_max)
    return false;

  auto intersection = r.at(t);
  Vec3 planar_hitpt = intersection - Q;
  auto a = dot(w, cross(planar_hitpt, v));
  auto b = dot(w, cross(u, planar_hitpt));

  if (!is_interior(a, b, rec))
    return false;

  rec.t = t;
  rec.p = intersection;
  rec.set_face_normal(r, normal);
  rec.mat_ptr = mat_ptr;

  return true;
}
bool quadilateral::is_interior(float a, float b, hitrecord &rec) const {
  interval unit_interval(0, 1);

  if (!unit_interval.contains(a) || !unit_interval.contains(b))
    return false;

  rec.uv = Vec2(a, b);
  return true;
}
shared_ptr<hittable_list> box(const point3 &corner1, const point3 &corner2,
                              const shared_ptr<material> &mat) {
  auto sides = make_shared<hittable_list>();

  auto min = glm::min(corner1, corner2);
  auto max = glm::max(corner1, corner2);

  auto dx = Vec3(max.x - min.x, 0, 0);
  auto dy = Vec3(0, max.y - min.y, 0);
  auto dz = Vec3(0, 0, max.z - min.z);

  sides->add(
      make_shared<quadilateral>(point3(min.x, min.y, max.z), dx, dy, mat));
  sides->add(
      make_shared<quadilateral>(point3(max.x, min.y, max.z), -dz, dy, mat));
  sides->add(
      make_shared<quadilateral>(point3(max.x, min.y, min.z), -dx, dy, mat));
  sides->add(
      make_shared<quadilateral>(point3(min.x, min.y, min.z), dz, dy, mat));
  sides->add(
      make_shared<quadilateral>(point3(min.x, max.y, max.z), dx, -dz, mat));
  sides->add(
      make_shared<quadilateral>(point3(min.x, max.y, max.z), dx, dz, mat));

  return sides;
}
