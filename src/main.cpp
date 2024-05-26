#include "common.h"

#include "hittable_list.h"
#include "sphere.h"

#include "Framebuffer.h"
#include "camera.h"

#include <iostream>

#include "material.h"
#include <execution>

using point3 = glm::vec3;
using color = glm::vec3;

hittable_list random_scene() {
  hittable_list world;

  auto ground_material = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
  world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000, ground_material));

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

  return world;
}

color ray_colour(const Ray &ray, const Hittable &world, int depth) {
  hitrecord rec = {};
  if (depth <= 0) {
    return color(0, 0, 0);
  }
  if (world.hit(ray, 0.001, infinity, rec)) {

    Ray scattered;
    color attenuation;
    if (rec.mat_ptr) {
      if (rec.mat_ptr->scatter(ray, rec, attenuation, scattered)) {
        return attenuation * ray_colour(scattered, world, depth - 1);
      } else {
        return color(0, 0, 0);
      }
    }
  }
  glm::vec3 unit_direction = glm::normalize(ray.direction());
  auto t = 0.5f * (unit_direction.y + 1.0f);
  return (1.0f - t) * color(1.0f, 1.0f, 1.0f) + t * color(0.5, 0.7, 1.0);
}

using point2 = glm::vec2;
#include <algorithm>
#include <chrono>

int main() {
  const auto aspect_ratio = 16.0f / 9.0f;
  const size_t width = 1920;
  const size_t height = static_cast<size_t>(width / aspect_ratio);
  Framebuffer fb(width, height);

  float viewport_height = 2.0f;
  float viewport_width = aspect_ratio * viewport_height;
  float focal_length = 1.0f;
  const int samples_per_pixel = 10;
  const int max_depth = 50;

  // world
  hittable_list world;

  auto ground_material = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
  world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000, ground_material));

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

  // auto ground_material = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
  // world.add(make_shared<Sphere>(point3(0,-1000,0), 1000, ground_material));

  //    Vertex v0 = {point3(0, 0, 0), point3(0, 0, 0)};
  //    Vertex v1 = {point3(1, 0, 0), point3(0, 0, 0)};
  //    Vertex v2 = {point3(0, 1, 0), point3(0, 0, 0)};
  //
  //        std::vector<Vertex>* vertices = new std::vector<Vertex>();
  //        vertices->push_back(v0);
  //        vertices->push_back(v1);
  //        vertices->push_back(v2);
  //        auto material = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
  //        world.add(make_shared<Triangle>(vertices, 0, 1, 2, material));

  //  auto vertexIndexPair = loadMesh("../WusonOBJ.obj");
  //
  //  std::vector<Vertex> vertices = vertexIndexPair.first;
  //  std::vector<int> indices = vertexIndexPair.second;
  //
  //  // really simple two triangle making a square
  //
  //  //  std::vector<Vertex> vertices = {
  //  //      Vertex(point3(0.25, 0.25, 0), point3(0, 0, 1), point2(0, 0)),
  //  //      Vertex(point3(0, 0.25, 0), point3(0, 0, 1), point2(1, 0)),
  //  //      Vertex(point3(0.25, 0, 0), point3(0, 0, 1), point2(1, 1)),
  //  //      Vertex(point3(0.25, 0.25, 0), point3(0, 0, 1), point2(0, 0)),
  //  //  };
  //  //
  //  //  std::vector<int> indices = {0, 1, 2};
  //
  //  Mesh mesh = Mesh(vertices, indices);
  //
  //  world.add(make_shared<Mesh>(mesh));

  point3 lookfrom(13, 2, 3);
  point3 lookat(0, 0, 0);

  glm::vec3 vup(0, 1, 0);
  float dist_to_focus = 10.0f;
  float aperture = 0.1f;

  camera cam(aspect_ratio, 2.0f, 20, lookfrom, lookat, vup, aperture,
             dist_to_focus);

  auto start = std::chrono::high_resolution_clock::now();

  std::vector<uint32_t> horizontalIterator, verticalIterator;
  horizontalIterator.resize(fb.getWidth());
  verticalIterator.resize(fb.getHeight());

  for (int i = 0; i < fb.getWidth(); i++)
    horizontalIterator[i] = i;
  for (int i = 0; i < fb.getHeight(); i++)
    verticalIterator[i] = i;

  int lines = 0;
#if MT
  std::for_each(std::execution::par, verticalIterator.begin(),
                verticalIterator.end(), [&](uint32_t y) {
                  std::for_each(
                      std::execution::par, horizontalIterator.begin(),
                      horizontalIterator.end(), [&, y](uint32_t x) {
                        color pixel_color(0, 0, 0);
                        for (int s = 0; s < samples_per_pixel; ++s) {
                          float u = (x + random_float()) / (width - 1);
                          float v = (y + random_float()) / (height - 1);

                          Ray r = cam.sendRay(u, v);
                          pixel_color += ray_colour(r, world, max_depth);
                        }
                        fb.setPixel(x, height - y, pixel_color.r, pixel_color.g,
                                    pixel_color.b, samples_per_pixel);
                      });
                });

#elif MT_LINES

  std::for_each(std::execution::par, verticalIterator.begin(),
                verticalIterator.end(), [&](uint32_t y) {
                  std::cerr << "\rScanlines done: " << y << ' ' << std::flush;
                  std::for_each(
                      horizontalIterator.begin(), horizontalIterator.end(),
                      [&, y](uint32_t x) {
                        color pixel_color(0, 0, 0);
                        for (int s = 0; s < samples_per_pixel; ++s) {
                          float u = (x + random_float()) / (width - 1);
                          float v = (y + random_float()) / (height - 1);

                          Ray r = cam.sendRay(u, v);
                          pixel_color += ray_colour(r, world, max_depth);
                        }
                        fb.setPixel(x, height - y, pixel_color.r, pixel_color.g,
                                    pixel_color.b, samples_per_pixel);
                      });
                });

#else
  for (size_t y = 0; y < fb.getHeight(); y++) {
    std::cerr << "\rScanlines done: " << y << ' ' << std::flush;
    for (size_t x = 0; x < fb.getWidth(); x++) {
      color pixel_color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s) {
        float u = (x + random_float()) / (width - 1);
        float v = (y + random_float()) / (height - 1);

        Ray r = cam.sendRay(u, v);
        pixel_color += ray_colour(r, world, max_depth);
      }
      fb.setPixel(x, height - y, pixel_color.r, pixel_color.g, pixel_color.b,
                  samples_per_pixel);
    }
  }
#endif

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cerr << "\nDone. Time taken: " << elapsed.count() << " seconds\n";

  fb.saveBuffer("test.png");
  system("feh");
}
