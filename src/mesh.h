
#pragma once
#include <iostream>
#include <utility>

#include "common.h"
#include "glm/geometric.hpp"
#include "hittable.h"
#include "quadilateral.h"

class Triangle : public Hittable {
public:
  Triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
           shared_ptr<material> mat)
      : v0(v0), v1(v1), v2(v2), mat(std::move(mat)) {
    // See Baldwin & Weber, "Fast Ray-Triangle Intersection by Coordinate
    // Transformation" for the derivation of the following matrix.
    Vec3 e1 = v1 - v0;
    Vec3 e2 = v2 - v0;
    Vec3 n = glm::cross(e1, e2);
    float maxComponent = std::max(std::max(fabs(n.x), fabs(n.y)), fabs(n.z));
    if (maxComponent == fabs(n.x)) {
      globalToBarycentric =
          Transform(0, e2.z / n.x, -e2.y / n.x, glm::cross(v2, v1).x / n.x, 0,
                    -e1.z / n.x, e1.y / n.x, -glm::cross(v1, v0).x / n.x, 1,
                    n.y / n.x, n.z / n.x, -dot(n, v0) / n.x, 0, 0, 0, 1);
    } else if (maxComponent == fabs(n.y)) {
      globalToBarycentric =
          Transform(-e2.z / n.y, 0, e2.x / n.y, glm::cross(v2, v1).y / n.y,
                    e1.z / n.y, 0, -e1.x / n.y, -glm::cross(v1, v0).y / n.y,
                    n.x / n.y, 1, n.z / n.y, -dot(n, v1) / n.y, 0, 0, 0, 1);
    } else {
      globalToBarycentric =
          Transform(e2.y / n.z, -e2.x / n.z, 0, glm::cross(v2, v1).z / n.z,
                    -e1.y / n.z, e1.x / n.z, 0, -glm::cross(v1, v0).z / n.z,
                    n.x / n.z, n.y / n.z, 1, -dot(n, v2) / n.z, 0, 0, 0, 1);
    }
  }

  void set_bounding_box() {
    Vec3 min = glm::min(v0, glm::min(v1, v2));
    Vec3 max = glm::max(v0, glm::max(v1, v2));
    bbox = aabb(min, max);
  }

  bool hit(const Ray &r, float t_min, float t_max,
           hitrecord &rec) const override;

  aabb bounding_box() const override { return bbox; }

private:
  Vec3 v0, v1, v2;
  aabb bbox;
  shared_ptr<material> mat;
  Transform globalToBarycentric;
};

class Mesh : public Hittable {
public:
  Mesh(const std::vector<Vec3> &vertices_, const std::vector<Vec3> &normals,
       const std::vector<Vec2> &uvs, const std::vector<int> &indices,
       shared_ptr<material> mat, Transform transform = Transform(1));

  Mesh(const char *filename, shared_ptr<material> mat,
       Transform transform = Transform(1));

  bool hit(const Ray &r, float t_min, float t_max,
           hitrecord &rec) const override;

  aabb bounding_box() const override { return bbox; }

  void set_bounding_box() {
    Vec3 min = vertices[0];
    Vec3 max = vertices[0];
    for (const auto &vertex : vertices) {
      min = glm::min(min, vertex);
      max = glm::max(max, vertex);
    }
    bbox = aabb(min, max);
  }

private:
  void transform_mesh(const Transform &transform);
  std::vector<Vec3> vertices;
  std::vector<Vec3> normals;
  std::vector<Vec2> uvs;
  std::vector<int> indices;
  std::vector<shared_ptr<Triangle>> triangles;
  shared_ptr<material> mat;
  aabb bbox;
};
