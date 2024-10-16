#pragma once
#include "Framebuffer.h"
#include "Ray.h"
#include "common.h"
#include "hittable.h"
#include "material.h"
#include <algorithm>
#include <cmath>
#include <execution>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <thread>

void printProgress(std::atomic<int> &counter, int total) {
  while (counter < total) {
    // estimate the progress by the counter
    std::cerr << "\rProgress: " << counter << "/" << total << ' ' << std::flush;
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}

class camera {
public:
    camera(){
    }

    Transform getTransform() {
    glm::mat4 view = glm::lookAt(lookfrom, lookat, vup);

    return view;
  }
  void render(const Hittable &world)
  {
      std::mutex framebuffer_mutex;
      render(world, framebuffer_mutex);
  }
void render(const Hittable& world, std::mutex& framebuffer_mutex)
{
    initCamera();

    // Define chunk size (e.g., 16x16 pixel chunks)
    const int chunk_size = 8;

    fb = new Framebuffer(image_width, image_height);

    // Vector to store chunk positions (x_start, y_start)
    std::vector<std::pair<int, int>> chunks;

    for (int y = 0; y < image_height; y += chunk_size) {
        for (int x = 0; x < image_width; x += chunk_size) {
            chunks.emplace_back(x, y);
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

#if MT
    std::atomic<int> counter = 0;

    // Use parallel execution to process each chunk
    std::for_each(std::execution::par, chunks.begin(), chunks.end(),
                  [&](std::pair<int, int> chunk)
                  {
                      int x_start = chunk.first;
                      int y_start = chunk.second;

                      // Create local pixel color storage for each thread
                      std::vector<std::pair<int, int>> pixels;

                      // Generate the list of pixels within this chunk
                      for (int y = y_start; y < std::min(y_start + chunk_size, image_height); ++y) {
                          for (int x = x_start; x < std::min(x_start + chunk_size, image_width); ++x) {
                              pixels.emplace_back(x, y);
                          }
                      }

                      // Process pixels inside the chunk in parallel
                      std::for_each(std::execution::par, pixels.begin(), pixels.end(),
                                    [&](std::pair<int, int> pixel)
                                    {
                                        int x = pixel.first;
                                        int y = pixel.second;

                                        color pixel_color(0, 0, 0);
                                        for (int s_j = 0; s_j < sqrt_samples; s_j++) {
                                            for (int s_i = 0; s_i < sqrt_samples; s_i++) {
                                                Ray r = sendRay(x, y, s_i, s_j);
                                                pixel_color += ray_colour(r, world, max_depth);
                                            }
                                        }

                                        // Lock framebuffer before writing to it
                                        std::lock_guard<std::mutex> guard(framebuffer_mutex);
                                        fb->setPixel(x, y, pixel_color.r, pixel_color.g, pixel_color.b, samples_per_pixel);
                                    });

                      // Increment counter after processing each chunk
                      counter += chunk_size * chunk_size;
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
                            Ray r = sendRay(x, y);
                            pixel_color += ray_colour(r, world, max_depth);
                          }
                          fb.setPixel(x, height - y, pixel_color.r,
                                      pixel_color.g, pixel_color.b,
                                      samples_per_pixel);
                        });
                  });

#else
    for (size_t y = 0; y < fb->getHeight(); y++) {
      std::cerr << "\rScanlines done: " << y << ' ' << std::flush;
      for (size_t x = 0; x < fb->getWidth(); x++) {
        color pixel_color(0, 0, 0);
        for (int s = 0; s < samples_per_pixel; ++s) {
          Ray r = sendRay(x, y);
          pixel_color += ray_colour(r, world, max_depth);
        }
        fb.setPixel(x, image_height - y, pixel_color.r, pixel_color.g,
                    pixel_color.b, samples_per_pixel);
      }
    }
#endif
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cerr << "\nDone. Time taken: " << elapsed.count() << " seconds\n";

    fb->saveBuffer("output.png");
}
    float aspectratio = 1.0f;
  int image_width = 1920;
  int image_height = 1080;
  int samples_per_pixel = 10;
  int sqrt_samples = 10;
  float recip_sqrt_samples;
  int max_depth = 10;

  color background;

  // TODO: Use matrices instead

  float fov = 90;
  point3 lookfrom = point3(0, 0, 0);
  point3 lookat = point3(0, 0, -1);
  Vec3 vup = Vec3(0, 1, 0);

  float defocus_angle = 0;
  float focus_dist = 10;

    Framebuffer* fb;
private:
  void initCamera() {
    image_height = int(image_width / aspectratio);
    image_height = (image_height < 1) ? 1 : image_height;

    sqrt_samples = int(sqrt(samples_per_pixel));
    pixel_samples_scale =
        1.0 / samples_per_pixel; // could be changed, essentially useless
    recip_sqrt_samples = 1.0f / sqrt_samples;

    center = lookfrom;

    auto theta = deg_to_reg(fov);
    float h = tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    auto viewport_width = viewport_height * (float(image_width) / image_height);

    w = glm::normalize(lookfrom - lookat);
    u = glm::normalize(glm::cross(vup, w));
    v = glm::cross(w, u);

    Vec3 viewport_u = viewport_width * u;
    Vec3 viewport_v = viewport_height * -v;

    pixel_delta_u = viewport_u / (float) image_width;
    pixel_delta_v = viewport_v / (float) image_height;

    auto viewport_upper_left =
        center - (focus_dist * w) - (viewport_u / 2.0f) - (viewport_v / 2.0f);

    pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

    auto defocus_radius = focus_dist * tan(glm::radians(defocus_angle / 2));
    defocus_disk_u = u * (float) defocus_radius;
    defocus_disk_v = v * (float) defocus_radius;
  }

  Ray sendRay(int i, int j, int s_i, int s_j) {
    auto offset = sample_square_stratified(s_i, s_j);
    auto pixel_sample =
        pixel00_loc + ((float) (i) * pixel_delta_u) + ((float) (j) * pixel_delta_v);

    glm::vec3 ray_origin =
        (defocus_angle <= 0) ? center : defocus_disk_sample();
    glm::vec3 ray_direction = pixel_sample - ray_origin;

    return Ray(ray_origin, ray_direction);
  }

  Vec3 sample_square() const {
    return Vec3(random_float() - 0.5, random_float() - 0.5, 0);
  }

  Vec3 sample_square_stratified(int s_i, int s_j) {
    auto px = ((s_i + random_float()) * recip_sqrt_samples) - 0.5f;
    auto py = ((s_j) + random_float() * recip_sqrt_samples) - 0.5f;

    return Vec3(px, py, 0);
  }

  point3 sample_disk(float radius) const {
    return radius * random_vector_in_unit_disk();
  }

  point3 defocus_disk_sample() const {
    auto p = random_vector_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }

  color ray_colour(const Ray &ray, const Hittable &world, int depth) {
    if (depth <= 0)
      return color(0, 0, 0);

    hitrecord rec = {};

    if (!world.hit(ray, 0.001, infinity, rec))
      return background;

    if (rec.mat_ptr == nullptr)
      return color(0, 0, 0);

    Ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.uv, rec.p);
    if (!rec.mat_ptr->scatter(ray, rec, attenuation, scattered))
      return emitted;

    color from_scatter = attenuation * ray_colour(scattered, world, depth - 1);

    return from_scatter + emitted;
  }

  float m_aspect_height;
  float m_aspect_width;
  double pixel_samples_scale;
  point3 m_origin;
  glm::vec3 horizontal;
  glm::vec3 vertical;
  glm::vec3 lower_left_corner;
  glm::vec3 w, u, v;
  glm::vec3 pixel_delta_u, pixel_delta_v;
  glm::vec3 defocus_disk_u, defocus_disk_v;
  glm::vec3 pixel00_loc;
  float lens_rad;
    point3 center; // camera center

};
