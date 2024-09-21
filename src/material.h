#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"
#include "texture.h"
#include <math.h>

struct hitrecord;

class material {
public:
  virtual ~material() = default;

  virtual color emitted(const point2 &uv, const point3 &p) const {
    return color(0, 0, 0);
  }

  virtual bool scatter(const Ray &r_in, const hitrecord &rec,
                       color &attenuation, Ray &scattered) const {
    return false;
  }
};

class Lambertian : public material {
public:
  Lambertian(const color &a) : tex(make_shared<solid_colour>(a)) {}
  Lambertian(shared_ptr<Texture> tex) : tex(tex) {}

  virtual bool scatter(const Ray &r_in, const hitrecord &rec,
                       color &attenuation, Ray &scattered) const override {
    auto scatter_dir = rec.normal + random_unit_vector();
    // catch degenerate case where the ray is bad
    if (near_zero(scatter_dir)) {
      scatter_dir = rec.normal;
    }
    scattered = Ray(rec.p, scatter_dir);
    attenuation = tex->value(rec.uv, rec.p);
    return true;
  }

public:
  shared_ptr<Texture> tex;
};

class Metal : public material {
public:
  Metal(const color &a, float fuzz) : m_albedo(a), fuzz(fuzz) {}

  virtual bool scatter(const Ray &r_in, const hitrecord &rec,
                       color &attenuation, Ray &scattered) const override {
    glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
    scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = m_albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

public:
  color m_albedo;
  float fuzz;
};

class dielectric : public material {
public:
  dielectric(float idx_of_refraction)
      : m_idx_of_refraction(idx_of_refraction) {}

  virtual bool scatter(const Ray &r_in, const hitrecord &rec,
                       color &attenuation, Ray &scattered) const override {
    attenuation = color(1.0, 1.0, 1.0);
    float refraction_ratio =
        rec.front_face ? (1.0f / m_idx_of_refraction) : m_idx_of_refraction;

    glm::vec3 unit_dir = glm::normalize(r_in.direction());
    float cos_theta = fmin(glm::dot(-unit_dir, rec.normal), 1.0f);
    float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0f;
    glm::vec3 dir;

    if (cannot_refract ||
        SchlickApprox(cos_theta, refraction_ratio) > random_float())
      dir = reflect(unit_dir, rec.normal);
    else
      dir = refract(unit_dir, rec.normal, refraction_ratio);

    scattered = Ray(rec.p, dir);
    return true;
  }

public:
  float m_idx_of_refraction;

private:
  static float SchlickApprox(float cosine, float ref_idx) {
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = pow(r0, 2);
    return r0 + (1 - r0) * pow((1 - cosine), 5);
  }
};

#endif // MATERIAL_H
