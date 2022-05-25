#include "common.h"

#include "hittable_list.h"
#include "sphere.h"

#include "camera.h"
#include "Framebuffer.h"

#include <iostream>

using point3 = glm::vec3;
using color = glm::vec3;


color ray_colour(const Ray& ray, const Hittable& world, int depth)
{
    hitrecord rec;
    if(depth <= 0)
    {
        return color(0, 0, 0);
    }
    if(world.hit(ray, 0, infinity, rec))
    {
        point3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5f * ray_colour(Ray(rec.p, target - rec.p), world, depth - 1);
    }
    glm::vec3 unit_direction = glm::normalize(ray.direction());
    auto t = 0.5f*(unit_direction.y + 1.0f);
    return (1.0f - t) * color(1.0f, 1.0f, 1.0f) + t * color(0.5, 0.7, 1.0);
}



int main() {
    const auto aspect_ratio = 16.0f / 9.0f;
    const size_t width = 1920;
    const size_t height = static_cast<size_t>(width / aspect_ratio);
    Framebuffer fb(width, height);

    float viewport_height = 2.0f;
    float viewport_width = aspect_ratio * viewport_height;
    float focal_length = 1.0f;
    const int samples_per_pixel = 10;
    const int max_depth = 25;

    // world
    hittable_list world;
    world.add(make_shared<Sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<Sphere>(point3(0, -100.5, -1), 100));

    camera cam(viewport_width, viewport_height);



    for(size_t y = 0; y < fb.getHeight(); y++)
    {
        std::cerr << "\rScanlines remaining: " << y << ' ' << std::flush;
        for(size_t x = 0; x < fb.getWidth(); x++)
        {
            color pixel_color(0, 0, 0);
            for(int s = 0; s < samples_per_pixel; ++s)
            {
                float u = (x + random_float()) / (width - 1);
                float v = (y + random_float()) / (height -1);

                Ray r = cam.sendRay(u, v, focal_length);
                pixel_color += ray_colour(r, world, max_depth);
            }
            fb.setPixel(x, height - y, pixel_color.r, pixel_color.g, pixel_color.b, samples_per_pixel);
        }
    }

    fb.saveBuffer("test.png");
    system("feh");
}
