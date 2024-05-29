//
// Created by sohaibalam on 27/05/24.
//

#ifndef RAYTRACINGBUTGOOD_QUADILATERAL_H
#define RAYTRACINGBUTGOOD_QUADILATERAL_H

#include "hittable.h"
#include "hittable_list.h"
class quadilateral : public Hittable {
public:
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

  Vec3 normal;

public:
  quadilateral(const Transform &objectToWorld, const point3 &Q_, const Vec3 &u_,
               const Vec3 &v_, shared_ptr<material> mat)
      : mat_ptr(mat) {

    Q = (objectToWorld * glm::vec4(Q_, 1)).xyz();
    u = (objectToWorld * glm::vec4(u_, 0)).xyz();
    v = (objectToWorld * glm::vec4(v_, 0)).xyz();
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
  float D; // some constant for the general plane equation
};

shared_ptr<hittable_list> box(const Transform &objectToWorld,
                              const point3 &corner1, const point3 &corner2,
                              const shared_ptr<material> &mat);

#endif // RAYTRACINGBUTGOOD_QUADILATERAL_H
