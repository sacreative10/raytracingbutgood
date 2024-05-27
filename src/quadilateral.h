//
// Created by sohaibalam on 27/05/24.
//

#ifndef RAYTRACINGBUTGOOD_QUADILATERAL_H
#define RAYTRACINGBUTGOOD_QUADILATERAL_H

#include "hittable.h"
#include "hittable_list.h"
class quadilateral : public Hittable {

  virtual void set_bounding_box() {
    auto bbox_diag1 = aabb(Q, Q + u + v);
    auto bbox_diag2 = aabb(Q + u, Q + v);
    bbox = aabb(bbox_diag1, bbox_diag2);
  }

  // this can be expanded for triangles very easily
  virtual bool is_interior(float a, float b, hitrecord &rec) const;

  aabb bounding_box() const override { return bbox; }

  bool hit(const Ray &r, float t_min, float t_max,
           hitrecord &rec) const override;

public:
  quadilateral(const point3 &Q, const Vec3 &u, const Vec3 &v,
               shared_ptr<material> mat)
      : Q(Q), u(u), v(v), mat_ptr(mat) {
    auto n = glm::cross(u, v);
    normal = glm::normalize(n);
    D = glm::dot(normal, Q);

    w = n / glm::dot(n, n);
    set_bounding_box();
  }

private:
  Point3 Q;
  Vec3 u, v;
  Vec3 w;
  std::shared_ptr<material> mat_ptr;
  aabb bbox;
  Vec3 normal;
  float D; // some constant for the general plane equation
};

shared_ptr<hittable_list> box(const point3 &corner1, const point3 &corner2,
                              const shared_ptr<material> &mat);

#endif // RAYTRACINGBUTGOOD_QUADILATERAL_H
