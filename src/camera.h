#include "Ray.h"


class camera
{
public:
    camera(float aspect_width, float aspect_height) : m_aspect_width(aspect_width), m_aspect_height(aspect_height), m_origin({0.0, 0.0, 0.0})  {}

    Ray sendRay(float u, float v, float focal_length)
    {
        const glm::vec3 horizontal { m_aspect_width, 0.0, 0.0 };
        const glm::vec3 vertical { 0.0, m_aspect_height, 0.0 };
        const glm::vec3 lower_left_corner = m_origin - horizontal/2.0f - vertical/2.0f - glm::vec3(0.0, 0.0, focal_length);
        return Ray(m_origin, lower_left_corner + u*horizontal + v*vertical - m_origin);
    }
private:
    float m_aspect_height;
    float m_aspect_width;
    point3 m_origin;

};