#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "Ray.h"

using Vec3 = glm::vec3;

class Triangle:public Hittable {
public:
    Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, shared_ptr<material> mat = nullptr)
        : vertex0(v0), vertex1(v1), vertex2(v2), m_mat(mat) {}

    
    virtual bool hit(const Ray& ray, float tMin, float tMax, hitrecord& record) const override {
        return intersect(ray, tMin, tMax, record);
    }

    bool intersect(const Ray& ray, float tMin, float tMax, hitrecord& record) const {
        // this uses the Möller–Trumbore intersection algorithm
        Vec3 edge1 = vertex1 - vertex0;
        Vec3 edge2 = vertex2 - vertex0;
        Vec3 ray_cross = glm::cross(ray.direction(), edge2);
        float det = glm::dot(edge1, ray_cross);

        if (det > -0.0001f && det < 0.0001f) {
            // ray and triangle are parallel
            return false;
        }

        float inv_det = 1.0f / det;
        Vec3 s = ray.origin() - vertex0;
        float u = inv_det * glm::dot(s, ray_cross);

        if (u < 0.0f || u > 1.0f) {
            return false;
        }

        Vec3 s_cross_e1 = cross(s, edge1);
        float v = inv_det * glm::dot(ray.direction(), s_cross_e1);

        if (v < 0.0f || u + v > 1.0f) {
            return false;
        }

        float t = inv_det * glm::dot(edge2, s_cross_e1);

        if (t < tMin || t > tMax) {
            return false;
        }

        record.t = t;   
        record.p = ray.at(t);
        record.set_face_normal(ray, glm::cross(edge1, edge2));
        record.mat_ptr = m_mat;
        return true;
    }

private:
    Vec3 vertex0;
    Vec3 vertex1;
    Vec3 vertex2;
    shared_ptr<material> m_mat;
};

#endif // TRIANGLE_H