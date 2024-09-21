#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"

class Triangle : public Hittable {
public:
    Triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, std::shared_ptr<material> mat)
        : v0(v0), v1(v1), v2(v2), mat(mat) {
        normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    }

    Triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, std::shared_ptr<material> mat, const Vec3 &normal)
        : v0(v0), v1(v1), v2(v2), mat(mat) {
        this->normal = normal;
    }


    virtual bool hit(const Ray &r, float t_min, float t_max, hitrecord &rec) const override;

    virtual aabb bounding_box() const override {
        Vec3 min_point(
            min(v0.x, v1.x, v2.x),
            min(v0.y, v1.y, v2.y),
            min(v0.z, v1.z, v2.z)
        );

        Vec3 max_point(
            max(v0.x, v1.x, v2.x),
            max(v0.y, v1.y, v2.y),
            max(v0.z, v1.z, v2.z)
        );

        return aabb(min_point, max_point);
    }

public:
    Vec3 v0, v1, v2;
    Vec3 normal;
    std::shared_ptr<material> mat;
};

#endif
