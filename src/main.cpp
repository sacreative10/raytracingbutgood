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
#include "hyperbolic.h"

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
    io.IniFilename = NULL;
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

  color col = color(0.8, 0.1, 0.8);
  color col2 = color(0.8, 0.8, 0.8);
  auto perlin_tex = make_shared<perlin_noise_texture_color>(4, col, col2);
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

  std::mutex frameBufferMutex;

  std::thread renderThread([&]() { cam.render(world, frameBufferMutex); });
  runGUI(cam, frameBufferMutex);
  renderThread.join();
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


  auto mesh = make_shared<Mesh>("../teapot.obj", make_shared<Lambertian>(color(1, 1, 1)), t);

  world.add(mesh);
  world = hittable_list(make_shared<bvh_node>(world));

  std::mutex frameBufferMutex;

  std::thread renderThread([&]() { cam.render(world, frameBufferMutex); });
    //runGUI(cam, frameBufferMutex);
    renderThread.join();
}


void scene1()
{
  hittable_list world;

  camera cam;

  cam.aspectratio = 1.0f;
  cam.image_width = 600;
  cam.samples_per_pixel = 200;
  cam.max_depth = 50;
  cam.background = color(0.f, 0.f, 0.f);

  cam.fov = 45;
  cam.lookfrom = point3(0, 1.3, -6);
  cam.lookat = point3(0, 0.5, 0);
  cam.vup = glm::vec3(0, 1, 0);

  cam.defocus_angle = 0;


  // ground col1 is orange
  color groundCol1 = color(0.49, 0.376, 0.165);
  // ground col2 is white
  color groundCol2 = color(0.753, 0.651, 0.216);

  auto kleinBottleInnerMat = make_shared<Metal>(color(0.8, 0.8, 0.8), 0.2f);

  color piColor = color(0.1, 0.1, 0.9);
  auto piMaterial = make_shared<Metal>(piColor, 0.001f);

  auto lightMat = make_shared<diffuseLights>(color(15, 15, 15));

  // ground sphere
  auto ground = make_shared<Sphere>(point3(0, -1000, 0), 1000,
                                    make_shared<Lambertian>(
                                        make_shared<perlin_noise_texture_color>(0.8, groundCol1, groundCol2)));


  Transform kleinBottleTOuter = glm::scale(glm::mat4(1), glm::vec3(0.1));
  kleinBottleTOuter = glm::rotate(kleinBottleTOuter, glm::radians(180.0f), glm::vec3(0, 1, 0));
  kleinBottleTOuter = glm::translate(kleinBottleTOuter, glm::vec3(0, 10, -2));


  auto kleinBottleOuter = make_shared<Mesh>("../kleinBottle.obj", make_shared<dielectric>(9.f), kleinBottleTOuter);

  Transform piT = glm::scale(glm::mat4(1), glm::vec3(0.5f));
  piT = glm::translate(piT, glm::vec3(2, 1, -2));

  auto pi = make_shared<Mesh>("../dodecahedron.obj", piMaterial, piT);


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
  auto light = make_shared<Sphere>(point3(0, 5, 0), 1, lightMat);

  world.add(ground);
  world.add(kleinBottleOuter);
  world.add(pi);
  world.add(light);
  world = hittable_list(make_shared<bvh_node>(world));

  std::mutex frameBufferMutex;
  std::thread renderThread([&]() { cam.render(world, frameBufferMutex); });
  runGUI(cam, frameBufferMutex);
  renderThread.join();
}


void chessScene()
{
  hittable_list world;

  camera cam;

  cam.aspectratio = 16.f / 9.f;
  cam.image_width = 1920;
  cam.samples_per_pixel = 400;
  cam.max_depth = 100;
  cam.background = color(0.8f, 0.8f, 0.8f);

  cam.fov = 45;
  cam.lookfrom = point3(0, 0.7, -7);
  cam.lookat = point3(0, 0.5, 0);
  cam.vup = glm::vec3(0, 1, 0);

  cam.defocus_angle = 0;

  auto groundMat = make_shared<Metal>(make_shared<checkered>(1.f, color(0.1f, 0.1f, 0.1f), color(0.9f, 0.9f, 0.9f)), 0.01f);
  auto ground = make_shared<Sphere>(point3(0, -1000, 0), 1000, groundMat);
  world.add(ground);

  auto knightColor = color(0.1, 0.1, 0.1);
  auto knightMat = make_shared<Lambertian>(knightColor);
  Transform knightT = glm::translate(glm::mat4(1), glm::vec3(0.5f, 0, -1.5f));
  knightT = glm::rotate(knightT, glm::radians(-90.0f), glm::vec3(0, 1, 0));
  knightT = glm::scale(knightT, glm::vec3(0.005f));
  auto knight = make_shared<Mesh>("../Knight.obj", knightMat, knightT);
  world.add(knight);

  auto kingColor = color(0.8, 0.8, 0.8);
  auto kingMat = make_shared<Lambertian>(kingColor);
  Transform kingT = glm::translate(glm::mat4(1), glm::vec3(-0.5f, 0, -2.5f));
  kingT = glm::rotate(kingT, glm::radians(-90.0f), glm::vec3(0, 1, 0));
  kingT = glm::scale(kingT, glm::vec3(0.0045f));
  auto king = make_shared<Mesh>("../king.obj", kingMat, kingT);
  world.add(king);

  color rightWallColor = color(.12, .45, .15);
  auto rightWallMat = make_shared<Metal>(rightWallColor, 0.01f);
  auto rightWall = make_shared<quadilateral>(Transform(1), point3(-1, -2, -6),
                                            glm::vec3(0, 0, 10), glm::vec3(0, 5, 0), rightWallMat);
  world.add(rightWall);

  auto leftWallColor = color(.65, .05, .05);
  auto leftWallMat = make_shared<Metal>(leftWallColor, 0.01f);
  auto leftWall = make_shared<quadilateral>(Transform(1), point3(1, -2, -6),
                                            glm::vec3(0, 0, 10), glm::vec3(0, 5, 0), leftWallMat);
  world.add(leftWall);

  auto backWallColor = color(.73, .73, .73);
  auto backWallMat = make_shared<Metal>(backWallColor, 0.01f);
  auto backWall = make_shared<quadilateral>(Transform(1), point3(2, -1, 2),
                                            glm::vec3(-10, 0, 0), glm::vec3(0, 10, 0), backWallMat);
  world.add(backWall);

  auto ceilingColor = color(.1, .1, .1);
  auto ceilingMat = make_shared<Metal>(ceilingColor, 0.01f);
  auto ceiling = make_shared<quadilateral>(Transform(1), point3(2, 3, -6),
                                           glm::vec3(-10, 0, 0), glm::vec3(0, 0, 10), ceilingMat);

  world.add(ceiling);

  auto lightMat = make_shared<diffuseLights>(color(15, 15, 15));
  auto light = make_shared<quadilateral>(Transform(1), point3(0.5, 2.9, -2),
                                         glm::vec3(-1, 0, 0), glm::vec3(0, 0, 2), lightMat);

  world.add(light);





  std::mutex frameBufferMutex;
  std::thread renderThread([&]() { cam.render(world, frameBufferMutex); });
  runGUI(cam, frameBufferMutex);
  renderThread.join();
}

void stanfordDragon()
{
  hittable_list world;

  camera cam;

  cam.aspectratio = 1.f;
  cam.image_width = 600;
  cam.samples_per_pixel = 400;
  cam.max_depth = 100;
  cam.background = color(0.8f, 0.8f, 0.8f);

  cam.fov = 45;
  cam.lookfrom = point3(0, 0.7, -7);
  cam.lookat = point3(0, 0.5, 0);
  cam.vup = glm::vec3(0, 1, 0);

  cam.defocus_angle = 0;

  auto groundMat = make_shared<Metal>(make_shared<checkered>(1.f, color(0.1f, 0.1f, 0.1f), color(0.9f, 0.9f, 0.9f)), 0.01f);
  auto ground = make_shared<Sphere>(point3(0, -1000, 0), 1000, groundMat);
 // world.add(ground);

  auto carColor = color(0.05, .05, .85);
  auto carMat = make_shared<Lambertian>(carColor);
  Transform carT = glm::translate(glm::mat4(1), glm::vec3(0, 0, -2));
  carT = glm::scale(carT, glm::vec3(0.2f));
  auto car = make_shared<Mesh>("../dragon.obj", carMat, carT);
  world.add(car);

  auto glassSphere = make_shared<Sphere>(point3(0, 0.5f, -5.5), 0.5f, make_shared<dielectric>(1.5f));
  world.add(glassSphere);

  auto leftWallColor = color(.12, .45, .15);
  auto leftWallMat = make_shared<Metal>(leftWallColor, 0.05f);
  auto leftWall = make_shared<quadilateral>(Transform(1), point3(-1.5, -2, -6),
                                            glm::vec3(0, 0, 10), glm::vec3(0, 5, 0), leftWallMat);
  world.add(leftWall);

  auto rightWallColor = color(.65, .05, .05);
  auto rightWallMat = make_shared<Metal>(rightWallColor, 0.05f);
  auto rightWall = make_shared<quadilateral>(Transform(1), point3(1.5, -2, -6),
                                             glm::vec3(0, 0, 10), glm::vec3(0, 5, 0), rightWallMat);
  world.add(rightWall);

  auto backWallColor = color(.73, .73, .73);
  auto backWallMat = make_shared<Metal>(backWallColor, 0.6f);
  auto backWall = make_shared<quadilateral>(Transform(1), point3(2, -1, 0.5),
                                            glm::vec3(-10, 0, 0), glm::vec3(0, 10, 0), backWallMat);

  world.add (backWall);

  auto ceilingColor = color(.1, .1, .1);
  auto ceilingMat = make_shared<Metal>(ceilingColor, 0.1f);
  auto ceiling = make_shared<quadilateral>(Transform(1), point3(2, 3, -6),
                                           glm::vec3(-20, 0, 0), glm::vec3(0, 0, 20), ceilingMat);
  world.add (ceiling);

  auto lightMat = make_shared<diffuseLights>(color(15, 15, 15));
  auto light = make_shared<quadilateral>(Transform(1), point3(0.5, 2.9, -4),
                                         glm::vec3(-1, 0, 0), glm::vec3(0, 0, 3), lightMat);
  world.add(light);

  auto floor = make_shared<quadilateral>(Transform(1), point3(2, -1, -6),
                                         glm::vec3(-10, 0, 0), glm::vec3(0, 0, 8),
                                         make_shared<Lambertian>(color(.73f, .73f, .73f)));

  world.add (floor);

  std::mutex frameBufferMutex;
  std::thread renderThread([&]() { cam.render(world, frameBufferMutex); });
  runGUI(cam, frameBufferMutex);
  renderThread.join();
}

void teapots()
{
  camera cam;

  hittable_list world;

  cam.aspectratio = 1.f;
  cam.image_width = 600;
  cam.samples_per_pixel = 800;
  cam.max_depth = 200;
  cam.background = color(0.8f, 0.8f, 0.8f);

  cam.fov = 45;
  cam.lookfrom = point3(1.9, 0.7, -1.9);
  cam.lookat = point3(-2, 0.1f, -6);
  cam.vup = glm::vec3(0, 1, 0);

  cam.defocus_angle = 0;

  auto groundMat = make_shared<Metal>(make_shared<image_texture>("../marbletiling.jpg"), 0.3f);
  auto groundQuad = make_shared<quadilateral>(Transform(1), point3(-2, 0, -6),
                                             glm::vec3(4, 0, 0), glm::vec3(0, 0, 4), groundMat);
  world.add(groundQuad);

  auto pinkTeapotCol = color(0.8, 0.1, 0.8);
  auto pinkTeapotMat = make_shared<Lambertian>(pinkTeapotCol);
  Transform pinkTeapotT = glm::translate(glm::mat4(1), glm::vec3(0, 0, -4.5));
  pinkTeapotT = glm::rotate(pinkTeapotT, glm::radians(-45.f), glm::vec3(0, 1, 0));
  pinkTeapotT = glm::scale(pinkTeapotT, glm::vec3(0.15f));
  auto pinkTeapot = make_shared<Mesh>("../teapot.obj", pinkTeapotMat, pinkTeapotT);

  auto blueTeapotCol = color(0.1, 0.1, 0.8);
  auto blueTeapotMat = make_shared<Lambertian>(blueTeapotCol);
  Transform blueTeapotT = glm::translate(glm::mat4(1), glm::vec3(-0.2, 0, -3.2));
  blueTeapotT = glm::rotate(blueTeapotT, glm::radians(-90.f), glm::vec3(0, 1, 0));
  blueTeapotT = glm::scale(blueTeapotT, glm::vec3(0.15f));
  auto blueTeapot = make_shared<Mesh>("../teapot.obj", blueTeapotMat, blueTeapotT);

  auto metalTeapotCol = color(0.1f);
  auto metalTeapotMat = make_shared<Metal>(metalTeapotCol, 0.001f);
Transform metalTeapotT = glm::translate(glm::mat4(1), glm::vec3(-1.f, 0, -4));
  metalTeapotT = glm::rotate(metalTeapotT, glm::radians(90.0f), glm::vec3(0, 1, 0));
  metalTeapotT = glm::scale(metalTeapotT, glm::vec3(0.2f));
  auto metalTeapot = make_shared<Mesh>("../teapot.obj", metalTeapotMat, metalTeapotT);


  auto glassSphere = make_shared<Sphere>(point3(1, 0.3, -3), 0.3f, make_shared<dielectric>(1.5f));





  world.add(pinkTeapot);
  world.add (blueTeapot);
  world.add(metalTeapot);
  world.add(glassSphere);

  auto wallColor = color(.8, .8, .8);
  auto wallMat = make_shared<Lambertian>(wallColor);
  auto leftWall = make_shared<quadilateral>(Transform(1), point3(-2, 0, -6),
                                            glm::vec3(0, 0, 4), glm::vec3(0, 5, 0), wallMat);
  auto rightWall = make_shared<quadilateral>(Transform(1), point3(2, 0, -6),
                                             glm::vec3(0, 0, 4), glm::vec3(0, 5, 0), wallMat);
  auto backWall = make_shared<quadilateral>(Transform(1), point3(-2, 0, -1),
                                            glm::vec3(4, 0, 0), glm::vec3(0, 5, 0), wallMat);
  auto ceiling = make_shared<quadilateral>(Transform(1), point3(-2, 5, -6),
                                           glm::vec3(-10, 0, 0), glm::vec3(0, 0, 10), wallMat);
  auto frontWall = make_shared<quadilateral>(Transform(1), point3(2, 0, -6),
                                             glm::vec3(-4, 0, 0), glm::vec3(0, 5, 0), wallMat);

  auto lightMat = make_shared<diffuseLights>(color(15, 15, 15));
  auto light = make_shared<quadilateral>(Transform(1), point3(-1.9, 0, -4),
                                         glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), lightMat);

  world.add(leftWall);
  world.add(rightWall);
  world.add(backWall);
  world.add(ceiling);
  world.add(frontWall);
  world.add(light);

  world = hittable_list(make_shared<bvh_node>(world));


  std::mutex frameBufferMutex;
  std::thread renderThread([&]() { cam.render(world, frameBufferMutex); });
  runGUI(cam, frameBufferMutex);
  renderThread.join();

}


void Lucy()
{
  camera cam;
  hittable_list world;

  cam.aspectratio = 1.f;
  cam.image_width = 600;
  cam.samples_per_pixel = 400;
  cam.max_depth = 100;
  cam.background = color(0.0f, 0.0f, 0.0f);

  cam.fov = 45;
  cam.lookfrom = point3(0, 2.f, -1);
  cam.lookat = point3(0, 1.f, 0);
  cam.vup = glm::vec3(0, 1, 0);

  cam.defocus_angle = 90.f;


  auto groundMat = make_shared<Metal>(make_shared<image_texture>("../marbletiling.jpg"), 0.3f);
  auto groundQuad = make_shared<quadilateral>(Transform(1), point3(-2, 0, -6),
                                             glm::vec3(4, 0, 0), glm::vec3(0, 0, 4), groundMat);

  // lucy will be of gold colour
  auto lucyColor = color(0.8, 0.6, 0.2);
  auto lucyMat = make_shared<Metal>(lucyColor, 0.1f);
  Transform lucyT = glm::scale(glm::mat4(1), glm::vec3(10.f));
  lucyT = glm::rotate (lucyT, glm::radians(180.0f), glm::vec3(0, 1, 0));
  lucyT = glm::translate(lucyT, glm::vec3(0, 0, 0));
  auto lucy = make_shared<Mesh>("../lucyHigh.obj", lucyMat, lucyT);


  auto lightMat = make_shared<diffuseLights>(color(15, 15, 15));
  // the light faces the model from the front
  auto lightT = glm::rotate(glm::mat4(1), glm::radians(-45.f), glm::vec3(1, 0, 0));
  lightT = glm::translate(lightT, glm::vec3(0, 1, -2));
  auto light = make_shared<quadilateral>(lightT, point3(1, 0, 1),
                                         glm::vec3(-2, 0, 0), glm::vec3(0, 0, -2), lightMat);
  world.add(light);

  //world.add(groundQuad);
  world.add(lucy);

  world = hittable_list(make_shared<bvh_node>(world));


  std::mutex frameBufferMutex;
  std::thread renderThread([&]() { cam.render(world, frameBufferMutex); });
  runGUI(cam, frameBufferMutex);
  renderThread.join();
}

void bust() {
  hittable_list world;

  camera cam;

  cam.aspectratio = 1.f;
  cam.image_width = 600;
  cam.samples_per_pixel = 1600;
  cam.max_depth = 400;
  cam.background = color(0.8f, 0.8f, 0.8f);

  cam.fov = 35;
  cam.lookfrom = point3(0, -0.2, -4);
  cam.lookat = point3(0, -0.5, 0);
  cam.vup = glm::vec3(0, 1, 0);

  cam.defocus_angle = 0;

  auto groundMat = make_shared<Metal>(make_shared<checkered>(1.f, color(0.1f, 0.1f, 0.1f), color(0.9f, 0.9f, 0.9f)), 0.01f);
  auto ground = make_shared<Sphere>(point3(0, -1000, 0), 1000, groundMat);
 // world.add(ground);

  auto carColor = color(0.05, .05, .05);
  auto carTex = make_shared<image_texture>("../marbleTex.jpg");
  auto carMat = make_shared<Metal>(carTex, 0.95f);
  Transform carT = glm::translate(glm::mat4(1), glm::vec3(0, -1, -2));
  carT = glm::rotate(carT, glm::radians(180.0f), glm::vec3(0, 1, 0));
  carT = glm::rotate(carT, glm::radians(270.f), glm::vec3(1, 0, 0));
  carT = glm::scale(carT, glm::vec3(0.9f));
  auto car = make_shared<Mesh>("../bust.obj", carMat, carT);
  world.add(car);

 // auto glassSphere = make_shared<Sphere>(point3(0, 0.5f, -5.5), 0.5f, make_shared<dielectric>(1.5f));
  //world.add(glassSphere);

  auto leftWallColor = color(.12 * 15, .15 * 15, .75 * 15);
  auto leftWallMat = make_shared<diffuseLights>(leftWallColor);
  auto leftWall = make_shared<quadilateral>(Transform(1), point3(-5, -2, -6),
                                            glm::vec3(0, 0, 5), glm::vec3(0, 5, 0), leftWallMat);
  world.add(leftWall);

  auto rightWallColor = color(.85 * 15, .05 * 15, .05 * 15);
  auto rightWallMat = make_shared<Metal>(rightWallColor, 0.05f);
  auto rightWall = make_shared<quadilateral>(Transform(1), point3(5, -2, -6),
                                             glm::vec3(0, 0, 5), glm::vec3(0, 5, 0), rightWallMat);
  world.add(rightWall);

  auto backWallColor = color(.73, .73, .73);
  auto backWallMat = make_shared<Metal>(backWallColor, 0.6f);
  auto backWall = make_shared<quadilateral>(Transform(1), point3(20, -5, 20),
                                            glm::vec3(-40, 0, 0), glm::vec3(0, 20, 0), backWallMat);

  world.add (backWall);

  auto ceilingColor = color(.1, .1, .1);
  auto ceilingMat = make_shared<Metal>(ceilingColor, 0.1f);
  auto ceiling = make_shared<quadilateral>(Transform(1), point3(2, 3, -6),
                                           glm::vec3(-20, 0, 0), glm::vec3(0, 0, 20), ceilingMat);
  //world.add (ceiling);

  auto lightMat = make_shared<diffuseLights>(color(15, 15, 15));
  auto light = make_shared<quadilateral>(Transform(1), point3(0.5, 4, 0),
                                         glm::vec3(-1, 0, 0), glm::vec3(0, 0, 3), lightMat);
  world.add(light);

  auto floor = make_shared<quadilateral>(Transform(1), point3(10, -1, -6),
                                         glm::vec3(-20, 0, 0), glm::vec3(0, 0, 20),
                                         make_shared<Lambertian>(color(.42f, .43f, .73f)));

  world.add (floor);

  world = hittable_list(make_shared<bvh_node>(world));

  std::mutex frameBufferMutex;
  std::thread renderThread([&]() { cam.render(world, frameBufferMutex); });
  runGUI(cam, frameBufferMutex);
  renderThread.join();
}

int main() { bust(); }
