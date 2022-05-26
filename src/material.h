#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"



struct hitrecord;

class material {
public:
    virtual bool scatter(
        const Ray& r_in, const hitrecord& rec, color& attenuation, Ray& scattered
     ) const = 0;
};

class Lambertian : public material
{
public:
    Lambertian(const color &a) : m_albedo(a){}

    virtual bool scatter(const Ray & r_in, const hitrecord& rec, color & attenuation, Ray & scattered) const override {
        auto scatter_dir = rec.normal + random_unit_vector();
        // catch degenerate case where the ray is bad
        if(near_zero(scatter_dir)) {
            scatter_dir = rec.normal;
        }
        scattered = Ray(rec.p, scatter_dir);
        attenuation = m_albedo;
        return true;
    }

public:
    color m_albedo;
};



class Metal : public material
{
public:
    Metal(const color& a) : m_albedo(a){}

    virtual bool scatter(const Ray & r_in, const hitrecord & rec, color & attenuation, Ray & scattered) const override
    {
        glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
        scattered = Ray(rec.p, reflected);
        attenuation = m_albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
public:
    color m_albedo;
};

#endif //MATERIAL_H
