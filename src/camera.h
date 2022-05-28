#pragma once
#include "Ray.h"


class camera
{
public:
    camera(float aspectratio, float aspect_height, float vfov, point3 lookfrom, point3 lookat, glm::vec3 vup, float aperture, float focus_dist){
        auto theta = deg_to_reg(vfov);
        float h = tan(theta/2);
        m_aspect_height = aspect_height * h;
        m_aspect_width = m_aspect_height * aspectratio;

        w = glm::normalize(lookfrom - lookat);
        u = glm::normalize(glm::cross(vup, w));
        v = glm::cross(w, u);

        m_origin = lookfrom;
        horizontal = m_aspect_width * u * focus_dist;
        vertical = m_aspect_height * v * focus_dist;
        lower_left_corner = m_origin - horizontal/2.0f - vertical/2.0f - focus_dist* w;

    }

    Ray sendRay(float s, float t)
    {
        glm::vec3 rd = lens_rad * random_vector_in_unit_disk();
        glm::vec3 offset = u * rd.x + v * rd.y;

        return Ray(
            m_origin + offset,
            lower_left_corner + s*horizontal + t*vertical - m_origin - offset);
    }
private:
    float m_aspect_height;
    float m_aspect_width;
    point3 m_origin;
    glm::vec3 horizontal;
    glm::vec3 vertical;
    glm::vec3 lower_left_corner;
    glm::vec3 w, u, v;
    float lens_rad;

};
