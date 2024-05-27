#include "common.h"

#include "hittable_list.h"
#include "image_texture.h"
#include "sphere.h"

#include "Framebuffer.h"
#include "camera.h"

#include <iostream>

#include "lights.h"
#include "material.h"
#include "perlin.h"
#include "quadilateral.h"
#include "texture.h"
#include "triangle.h"
#include <algorithm>
#include <chrono>
#include <execution>
#include <memory>

using point3 = glm::vec3;
using color = glm::vec3;

void firstBook() {
  hittable_list world;

  auto checker =
      make_shared<checkered>(0.32, color(.2, .3, .1), color(.9, .9, .9));
  world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000,
                                make_shared<Lambertian>(checker)));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_float();
      point3 center(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

      if ((center - point3(4, 0.2, 0)).length() > 0.9) {
        shared_ptr<material> sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = randomVector() * randomVector();
          sphere_material = make_shared<Lambertian>(albedo);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = randomVector(0.5f, 1.0f);
          auto fuzz = random_float(0, 0.5);
          sphere_material = make_shared<Metal>(albedo, fuzz);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          sphere_material = make_shared<dielectric>(1.5);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1 = make_shared<dielectric>(1.5);
  world.add(make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

  auto material2 = make_shared<Lambertian>(color(0.4, 0.2, 0.1));
  world.add(make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<Metal>(color(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));

  camera cam;

  cam.aspectratio = 16.0f / 9.0f;
  cam.image_width = 1200;
  cam.samples_per_pixel = 10;
  cam.max_depth = 20;

  cam.fov = 20;
  cam.lookfrom = point3(13, 2, 3);
  cam.lookat = point3(0, 0, 0);
  cam.vup = Vec3(0, 1, 0);

  cam.defocus_angle = 0.6;
  cam.focus_dist = 10.0;
  cam.background = color(0.7, 0.8, 1.0);

  cam.render(world);
}
void checkered_spheres() {
  hittable_list world;

  auto checker =
      make_shared<checkered>(0.32, color(.2, .3, .1), color(.9, .9, .9));

  world.add(make_shared<Sphere>(point3(0, -10, 0), 10,
                                make_shared<Lambertian>(checker)));
  world.add(make_shared<Sphere>(point3(0, 10, 0), 10,
                                make_shared<Lambertian>(checker)));

  camera cam;

  cam.aspectratio = 16.0 / 9.0;
  cam.image_width = 1200;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;

  cam.fov = 20;
  cam.lookfrom = point3(13, 2, 3);
  cam.lookat = point3(0, 0, 0);
  cam.vup = Vec3(0, 1, 0);
  cam.background = color(0.7, 0.8, 1.0);

  cam.defocus_angle = 0;

  cam.render(world);
}

void earth() {
  auto earth_texture = make_shared<image_texture>("../earthmap.jpg");
  auto earth_surface = make_shared<Lambertian>(earth_texture);

  auto globe = make_shared<Sphere>(point3(0, 0, 0), 2, earth_surface);

  camera cam;

  cam.aspectratio = 16.0 / 9.0;
  cam.image_width = 1200;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;

  cam.fov = 20;
  cam.lookfrom = point3(0, 0, 12);
  cam.lookat = point3(0, 0, 0);
  cam.vup = Vec3(0, 1, 0);
  cam.background = color(0.7, 0.8, 1.0);

  cam.defocus_angle = 0;

  cam.render(hittable_list(globe));
}

void perlin_spheres() {
  hittable_list world;

  auto perlin_tex = make_shared<perlin_noise_texture>(4);
  world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000,
                                make_shared<Lambertian>(perlin_tex)));
  world.add(make_shared<Sphere>(point3(0, 2, 0), 2,
                                make_shared<Lambertian>(perlin_tex)));

  camera cam;

  cam.aspectratio = 16.0 / 9.0;
  cam.image_width = 1200;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;

  cam.fov = 20;
  cam.lookfrom = point3(13, 2, 3);
  cam.lookat = point3(0, 0, 0);
  cam.vup = Vec3(0, 1, 0);
  cam.background = color(0.7, 0.8, 1.0);

  cam.defocus_angle = 0;

  cam.render(world);
}

void quads() {
  hittable_list world;

  auto left_red = make_shared<Lambertian>(color(0.8, 0.1, 0.1));
  auto back_green = make_shared<Lambertian>(color(0.1, 0.8, 0.1));
  auto right_blue = make_shared<Lambertian>(color(0.1, 0.1, 0.8));
  auto upper_orange = make_shared<Lambertian>(color(0.8, 0.8, 0.1));
  auto lower_cyan = make_shared<Lambertian>(color(0.1, 0.8, 0.8));

  world.add(make_shared<quadilateral>(point3(-3, -2, 5), Vec3(0, 0, -4),
                                      Vec3(0, 4, 0), left_red));
  world.add(make_shared<quadilateral>(point3(-2, -2, 0), Vec3(4, 0, 0),
                                      Vec3(0, 4, 0), back_green));
  world.add(make_shared<quadilateral>(point3(3, -2, 1), Vec3(0, 0, 4),
                                      Vec3(0, 4, 0), right_blue));
  world.add(make_shared<quadilateral>(point3(-2, 3, 1), Vec3(4, 0, 0),
                                      Vec3(0, 0, 4), upper_orange));
  world.add(make_shared<quadilateral>(point3(-2, -3, 5), Vec3(4, 0, 0),
                                      Vec3(0, 0, -4), lower_cyan));

  world.add(make_shared<Triangle>(point3(0, 0, 0), Vec3(1, 0, 0), Vec3(0, 1, 0),
                                  left_red));

  camera cam;

  cam.aspectratio = 16.0 / 9.0;
  cam.image_width = 1200;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;

  cam.fov = 80;
  cam.lookfrom = point3(0, 0, 9);
  cam.lookat = point3(0, 0, 0);
  cam.vup = Vec3(0, 1, 0);
  cam.background = color(0.7, 0.8, 1.0);

  cam.defocus_angle = 0;
  cam.render(world);
}

void light() {
  hittable_list world;

  auto perlin_tex = make_shared<perlin_noise_texture>(4);

  auto diffuse_lightTex = make_shared<diffuseLights>(color(5, 5, 5));

  world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000,
                                make_shared<Lambertian>(perlin_tex)));
  world.add(make_shared<Sphere>(point3(0, 2, 0), 2,
                                make_shared<Lambertian>(perlin_tex)));
  world.add(make_shared<quadilateral>(point3(3, 1, -2), Vec3(2, 0, 0),
                                      Vec3(0, 2, 0), diffuse_lightTex));

  camera cam;

  cam.aspectratio = 16.0 / 9.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;
  cam.background = color(0, 0, 0);

  cam.fov = 20;
  cam.lookfrom = point3(26, 3, 6);
  cam.lookat = point3(0, 2, 0);
  cam.vup = Vec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world);
}

// empty for now
void cornell_box() {
  hittable_list world;

  using vec3 = glm::vec3;
  using quad = quadilateral;
  auto red = make_shared<Lambertian>(color(.65, .05, .05));
  auto white = make_shared<Lambertian>(color(.73, .73, .73));
  auto green = make_shared<Lambertian>(color(.12, .45, .15));
  auto light = make_shared<diffuseLights>(color(15, 15, 15));

  world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0),
                              vec3(0, 0, 555), green));
  world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555),
                              red));
  world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0),
                              vec3(0, 0, -105), light));
  world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555),
                              white));
  world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0),
                              vec3(0, 0, -555), white));
  world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0),
                              vec3(0, 555, 0), white));

  world.add(box(point3(130, 0, 65), point3(295, 165, 230), white));
  world.add(box(point3(265, 0, 295), point3(430, 330, 460), white));

  camera cam;

  cam.aspectratio = 1.0;
  cam.image_width = 600;
  cam.samples_per_pixel = 200;
  cam.max_depth = 50;
  cam.background = color(0, 0, 0);

  cam.fov = 40;
  cam.lookfrom = point3(278, 278, -800);
  cam.lookat = point3(278, 278, 0);
  cam.vup = vec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world);
}

int main() { cornell_box(); }
