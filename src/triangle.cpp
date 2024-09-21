//
// Created by sacre on 19/08/2024.
//
#include "triangle.h"

bool Triangle::hit(const Ray &r, float t_min, float t_max, hitrecord &rec) const {
    // Möller-Trumbore intersection algorithm
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    Vec3 h = cross(r.direction(), edge2);
    float a = dot(edge1, h);

    if (std::abs(a) < std::numeric_limits<float>::epsilon()) {
        return false; // This ray is parallel to the triangle
    }

    float f = 1.0f / a;
    Vec3 s = r.origin() - v0;
    float u = f * dot(s, h);

    if (u < 0.0f || u > 1.0f) {
        return false; // The intersection lies outside of the triangle
    }

    Vec3 q = cross(s, edge1);
    float v = f * dot(r.direction(), q);

    if (v < 0.0f || u + v > 1.0f) {
        return false; // The intersection lies outside of the triangle
    }

    // At this stage we can compute t to find out where the intersection point is on the line
    float t = f * dot(edge2, q);

    if (t < t_min || t > t_max) {
        return false; // Intersection outside of bounds
    }

    // Ray intersection
    rec.t = t;
    rec.p = r.at(t);
    rec.normal = normal;
    rec.uv = {u, v};
    rec.mat_ptr = mat;

    return true;
}

