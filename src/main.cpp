#include "common.h"

#include "hittable_list.h"
#include "image_texture.h"
#include "sphere.h"

#include "Framebuffer.h"
#include "camera.h"

#include <iostream>

#include "bvh_node.h"
#include "lights.h"
#include "material.h"
#include "mesh.h"
#include "perlin.h"
#include "quadilateral.h"
#include "texture.h"
// #include "triangle.h"
#include "data.h"
#include <algorithm>
#include <chrono>
#include <execution>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using point3 = glm::vec3;
using color = glm::vec3;


void runGUI(camera& cam, std::mutex& frameBufferMutex)
{

    GLFWwindow* window;


    if (!glfwInit())
        return;

    window = glfwCreateWindow(cam.image_width, cam.image_height, "Raytracing", NULL, NULL);

    glfwMakeContextCurrent(window);



    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cam.image_width, cam.image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool shutDown = false;

    while (!glfwWindowShouldClose(window))
    {
        {
            std::lock_guard<std::mutex> lock(frameBufferMutex);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cam.image_width, cam.image_height, GL_RGB, GL_UNSIGNED_BYTE, cam.fb->getRawData());
        }

        glfwPollEvents();
        // see if the user wants to close the window by clicking the x button
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            shutDown = true;
        }
        // start imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Triangle Test", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImGui::Image((void*)(intptr_t)texture, ImVec2(cam.image_width + 10, cam.image_height + 10));
        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, cam.image_width, cam.image_height);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void firstBook()
{
  hittable_list world;

  auto checker =
    make_shared<checkered>(0.32, color(.2, .3, .1), color(.9, .9, .9));
  world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000,
                                make_shared<Lambertian>(checker)));

  for (int a = -11; a < 11; a++)
  {
    for (int b = -11; b < 11; b++)
    {
      auto choose_mat = random_float();
      point3 center(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

      if ((center - point3(4, 0.2, 0)).length() > 0.9)
      {
        shared_ptr<material> sphere_material;

        if (choose_mat < 0.8)
        {
          // diffuse
          auto albedo = randomVector() * randomVector();
          sphere_material = make_shared<Lambertian>(albedo);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        }
        else if (choose_mat < 0.95)
        {
          // metal
          auto albedo = randomVector(0.5f, 1.0f);
          auto fuzz = random_float(0, 0.5);
          sphere_material = make_shared<Metal>(albedo, fuzz);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        }
        else
        {
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

  world = hittable_list(make_shared<bvh_node>(world));

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

  std::mutex frameBufferMutex;
  std::thread renderThread([&]() { cam.render(world); });

    runGUI(cam, frameBufferMutex);
    renderThread.join();

}

void checkered_spheres()
{
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

void earth()
{
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

void perlin_spheres()
{
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

void light()
{
  hittable_list world;

  auto perlin_tex = make_shared<perlin_noise_texture>(4);

  auto diffuse_lightTex = make_shared<diffuseLights>(color(5, 5, 5));
  auto spot_light = make_shared<spotLight>(color(5, 5, 5), 10, 5);

  world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000,
                                make_shared<Lambertian>(perlin_tex)));
  world.add(make_shared<Sphere>(point3(0, 2, 0), 2,
                                make_shared<Lambertian>(perlin_tex)));
  world.add(make_shared<Sphere>(point3(0, 10, 0), 2, spot_light));

  world = hittable_list(make_shared<bvh_node>(world));

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
void cornell_box()
{
  hittable_list world;

  using vec3 = glm::vec3;
  using quad = quadilateral;
  auto red = make_shared<Lambertian>(color(.65, .05, .05));
  auto white = make_shared<Lambertian>(color(.73, .73, .73));
  auto green = make_shared<Lambertian>(color(.12, .45, .15));
  auto light = make_shared<diffuseLights>(color(15, 15, 15));

  /*
  world.add(make_shared<quad>(Transform(1), point3(555, 0, 0), vec3(0, 555, 0),
                              vec3(0, 0, 555), green));
  world.add(make_shared<quad>(Transform(1), point3(0, 0, 0), vec3(0, 555, 0),
                              vec3(0, 0, 555), red));
  */
  world.add(make_shared<quad>(Transform(1), point3(343, 554, 332),
                              vec3(-130, 0, 0), vec3(0, 0, -105), light));
  /*
  world.add(make_shared<quad>(Transform(1), point3(0, 0, 0), vec3(555, 0, 0),
                              vec3(0, 0, 555), white));
  world.add(make_shared<quad>(Transform(1), point3(555, 555, 555),
                              vec3(-555, 0, 0), vec3(0, 0, -555), white));
  world.add(make_shared<quad>(Transform(1), point3(0, 0, 555), vec3(555, 0, 0),
                              vec3(0, 555, 0), white));
                              */

  // Transform box1trans = glm::translate(glm::mat4(1), vec3(130, 0, 65));
  // Transform box1rot =
  //     glm::rotate(glm::mat4(1), glm::radians(-18.f), vec3(0, 1, 0));
  // Transform box1 = box1trans * box1rot;
  //
  // Transform box2trans = glm::translate(glm::mat4(1), vec3(265, 0, 295));
  // Transform box2rot =
  //     glm::rotate(glm::mat4(1), glm::radians(15.f), vec3(0, 1, 0));
  // Transform box2 = box2trans * box2rot;
  //
  // world.add(box(box1, point3(0, 0, 0), point3(165, 165, 165), white));
  // world.add(box(box2, point3(0, 0, 0), point3(165, 330, 165), white));

  Transform t = glm::scale(glm::mat4(1), glm::vec3(1));
  t = glm::translate(glm::mat4(1), vec3(130, 0, 65));

  auto teapot = make_shared<Mesh>("../WusonOBJ.obj", white, t);

  world.add(teapot);

  world = hittable_list(make_shared<bvh_node>(world));

  camera cam;

  cam.aspectratio = 1.0;
  cam.image_width = 600;
  cam.samples_per_pixel = 200;
  cam.max_depth = 50;
  cam.background = color(1, 0, 0);

  cam.fov = 40;
  cam.lookfrom = point3(278, 278, -800);
  cam.lookat = point3(278, 278, 0);
  cam.vup = vec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world);
}

void modelTest()
{
  hittable_list world;

  // basic light
  auto light = make_shared<diffuseLights>(color(15, 15, 15));

  // add light
  world.add(make_shared<quadilateral>(Transform(1), point3(343, 554, 332),
                                      glm::vec3(-130, 0, 0),
                                      glm::vec3(0, 0, -105), light));

  auto red = make_shared<Lambertian>(color(.65, .05, .05));

  // render just one triangle
  // world.add(make_shared<Triangle>(point3(0, 0, 0), point3(0, 0, 1),
  //                                 point3(1, 0, 0), red));

  camera cam;

  cam.aspectratio = 1.0;
  cam.image_width = 600;
  cam.samples_per_pixel = 200;
  cam.max_depth = 50;
  cam.background = color(0, 0, 0);

  cam.fov = 40;
  cam.lookfrom = point3(278, 278, -800);
  cam.lookat = point3(278, 278, 0);
  cam.vup = glm::vec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world);
}

void triangleTest()
{
  hittable_list world;

  camera cam;

  cam.aspectratio = 1.0f;
  cam.image_width = 600;
  cam.samples_per_pixel = 200;
  cam.max_depth = 50;
  cam.background = color(0, 0, 0);

  cam.fov = 40;
  cam.lookfrom = point3(0, 0, -6);
  cam.lookat = point3(0, 0, 0);
  cam.vup = glm::vec3(0, 1, 0);

  cam.defocus_angle = 0;

  auto light = make_shared<diffuseLights>(color(15, 15, 15));


  auto lightsource = make_shared<quadilateral>(Transform(1), point3(0.25, 1.5, -0.5),
                                               glm::vec3(-.75f, 0, 0), glm::vec3(0, 0, .75f), light);

  auto leftWall = make_shared<quadilateral>(Transform(1), point3(1.5, -2, -3),
                                            glm::vec3(0, 0, 10), glm::vec3(0, 5, 0),
                                            make_shared<Lambertian>(color(0.65f, .05f, .05f)));
  auto rightWall = make_shared<quadilateral>(Transform(1), point3(-1.5, -2, -3),
                                             glm::vec3(0, 0, 10), glm::vec3(0, 5, 0),
                                             make_shared<Lambertian>(color(.12f, .45f, .15f)));

  auto floor = make_shared<quadilateral>(Transform(1), point3(2, -1, -3),
                                         glm::vec3(-10, 0, 0), glm::vec3(0, 0, 10),
                                         make_shared<Lambertian>(color(.73f, .73f, .73f)));

  auto ceiling = make_shared<quadilateral>(Transform(1), point3(2, 1.5, -3),
                                           glm::vec3(-10, 0, 0), glm::vec3(0, 0, 10),
                                           make_shared<Lambertian>(color(.73f, .73f, .73f)));

  auto backWall = make_shared<quadilateral>(Transform(1), point3(2, -5, 3),
                                            glm::vec3(-10, 0, 0), glm::vec3(0, 10, 0),
                                            make_shared<Lambertian>(color(.73f, .73f, .73f)));

  world.add(leftWall);
  world.add(rightWall);
  world.add(floor);
  world.add(lightsource);
  world.add(backWall);
  world.add(ceiling);

  Transform t = glm::scale(glm::mat4(1), glm::vec3(0.8));
  t = glm::rotate(t, glm::radians(180.0f), glm::vec3(0, 1, 0));
  t = glm::rotate(t, glm::radians(30.f), glm::vec3(1, 0, 0));

  t = glm::translate(t, glm::vec3(0, 0, -5));


  auto mesh = make_shared<Mesh>("../suzanne.obj", make_shared<Lambertian>(color(1, 1, 1)), t);

  world.add(mesh);
  world = hittable_list(make_shared<bvh_node>(world));

  std::mutex frameBufferMutex;

  std::thread renderThread([&]() { cam.render(world, frameBufferMutex); });
    runGUI(cam, frameBufferMutex);
    renderThread.join();


}

int main() { firstBook(); }
