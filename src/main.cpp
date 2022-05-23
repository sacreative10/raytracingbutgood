#include <cstddef>
#include <cstdint>
#include <iostream>
#include "Framebuffer.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include <glm/glm.hpp>


#include "Ray.h"
#include "camera.h"

using point3 = glm::vec3;
using color = glm::vec3;


color ray_colour(const Ray& ray)
{
    float t = hitSphere(point3(0, 0, -1), 0.5, ray);
    if(t > 0.0)
    {
        glm::vec3 N = glm::normalize(ray.at(t) - point3(0, 0, -1));
        return 0.5f * color(N.x + 1, N.y + 1, N.z + 1);
    }
    glm::vec3 unit_direction = glm::normalize(ray.direction());
    t = 0.5f * (unit_direction.y + 1.0f);
    return float((1.0 - t))*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}



int main() {
    const auto aspect_ratio = 16.0f / 9.0f;
    const size_t width = 1920;
    const size_t height = static_cast<size_t>(width / aspect_ratio);
    Framebuffer fb(width, height);

    float viewport_height = 2.0f;
    float viewport_width = aspect_ratio * viewport_height;
    float focal_length = 1.0f;

    camera cam(viewport_width, viewport_height);



    for(size_t y = 0; y < fb.getHeight(); y++)
    {
        for(size_t x = 0; x < fb.getWidth(); x++)
        {
            auto u = float(x) / (fb.getWidth() - 1);
            auto v = float(y) / (fb.getHeight() - 1);

            color pixel_color = ray_colour(cam.sendRay(u, v, focal_length));


            fb.setPixel(x, height - y, (uint8_t)(255.99*pixel_color.r), (uint8_t)(255.99*pixel_color.g), (uint8_t)(255.99*pixel_color.b));
        }
    }

    fb.saveBuffer("test.png");
    system("feh");
}
