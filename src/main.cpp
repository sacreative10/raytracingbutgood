#include "common.h"

#include "hittable_list.h"
#include "sphere.h"

#include "camera.h"
#include "Framebuffer.h"

using point3 = glm::vec3;
using color = glm::vec3;


color ray_colour(const Ray& ray, const Hittable& world)
{
    hitrecord rec;
    if(world.hit(ray, 0, infinity, rec))
    {
        return 0.5f*(rec.normal + color(1, 1, 1));
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

    // world
    hittable_list world;
    world.add(make_shared<Sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<Sphere>(point3(0, -100.5, -1), 100));

    camera cam(viewport_width, viewport_height);



    for(size_t y = 0; y < fb.getHeight(); y++)
    {
        for(size_t x = 0; x < fb.getWidth(); x++)
        {
            auto u = float(x) / (fb.getWidth() - 1);
            auto v = float(y) / (fb.getHeight() - 1);

            color pixel_color = ray_colour(cam.sendRay(u, v, focal_length), world);


            fb.setPixel(x, height - y, (uint8_t)(255.99*pixel_color.r), (uint8_t)(255.99*pixel_color.g), (uint8_t)(255.99*pixel_color.b));
        }
    }

    fb.saveBuffer("test.png");
    system("feh");
}
