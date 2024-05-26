
#include "mesh.h"
#include <iostream>

bool Triangle::hit(const Ray &ray, float tMin, float tMax,
                   hitrecord &rec) const {
  // following the Möller–Trumbore intersection algorithm
  // https://ruurdsdevlog.wordpress.com/2020/03/04/adding-triangle-meshes-raytracer-2/

  Vec3 edge1, edge2, h, s, q, ca, cb, cc;
  float a, f, u, v;

  ca = vertices->at(v0).position;
  cb = vertices->at(v1).position;
  cc = vertices->at(v2).position;

  edge1 = cb - ca;
  edge2 = cc - ca;

  h = cross(ray.direction(), edge2);
  a = dot(edge1, h);

  if (a > EPSILON && a < EPSILON) {
    // the ray is parallel to the triangle
    return false;
  }

  f = 1.0 / a;
  s = ray.origin() - ca;
  u = f * dot(s, h);
  if (u < 0.0 || u > 1.0)
    return false;

  q = cross(s, edge1);
  v = f * dot(ray.direction(), q);
  if (v < 0.0 || u + v > 1.0)
    return false;

  // At this stage we can compute t to find out where the intersection point is
  // on the line.
  float t = f * dot(edge2, q);
  if (t > EPSILON && t < 1 / EPSILON) // ray intersection
  {
    if (t < tMax && t > tMin) // object closer than previous
    {
      rec.t = t;
      rec.p = ray.origin() + ray.direction() * t;
      rec.normal = this->getNormal();
      rec.mat_ptr = mat;
      return true;
    } else
      return false; // this means there is a ray intersection, but it is behind
                    // a different object
  } else {
    return false;
  }
}

Vec3 Triangle::getNormal() const {
  Vec3 edge1 = vertices->at(v1).position - vertices->at(v0).position;
  Vec3 edge2 = vertices->at(v2).position - vertices->at(v0).position;
  return glm::normalize(glm::cross(edge1, edge2));
}

bool Mesh::hit(const Ray &ray, float tMin, float tMax, hitrecord &rec) const {
  bool hit_anything = false;
  hitrecord temp_rec;
  float closest_so_far = tMax;

  // TODO: Add a system for different materials (could be done by a having a
  // materials index)

  // set all triangle vertex pointers
  for (auto &triangle : triangles) {
    // check if the normal is pointing away the camera, if so, skip
    if (glm::dot(triangle->getNormal(), ray.direction()) < 0) {
      continue;
    }
    if (triangle->hit(ray, tMin, closest_so_far, temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }

  return hit_anything;
}