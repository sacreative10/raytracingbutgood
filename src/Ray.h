#pragma once


#include <glm/glm.hpp>
#include "common.h"
using point3 = glm::vec3;
using color = glm::vec3;
class Ray
{
public:
    Ray(){}
    Ray(const point3& origin, const glm::vec3& direction) : m_origin(origin), m_direction(direction){}

    // getters
    point3 origin() const { return m_origin; }
    glm::vec3 direction() const { return m_direction; }

    point3 at(float t) const {
        return m_origin + t*m_direction;
    }

private:
    point3 m_origin;
    glm::vec3 m_direction;
};

inline float squareVector(const glm::vec3& v) {
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

inline float hitSphere(const point3& center, float radius, const Ray& r) {
    point3 oc = r.origin() - center;
    float a = squareVector(r.direction());
    float half_b = dot(oc, r.direction());
    float c = squareVector(oc) - radius*radius;
    auto discriminant = half_b * half_b- a*c;
    if(discriminant < 0) {
        return -1.0f;
    } else {
        return (-half_b - sqrt(discriminant))/a;
    }

}

inline glm::vec3 randomVector()
{
    return glm::vec3{random_float(), random_float(), random_float()};
}
inline glm::vec3 randomVector(float min, float max)
{
    return glm::vec3{random_float(min, max), random_float(min, max), random_float(min, max)};
}

inline glm::vec3 random_in_unit_sphere()
{
    while(true)
    {
        auto p = randomVector(-1, 1);
        if(squareVector(p) >= 1) continue;
        return p;
    }
}
