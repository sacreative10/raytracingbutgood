cmake_minimum_required(VERSION 3.0)

project(raytracingbutgood)

file(GLOB SOURCES src/*.cpp)

add_subdirectory(external/glm)
add_subdirectory(external/assimp)
add_compile_definitions(MT)
find_package(TBB REQUIRED COMPONENTS tbb)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(raytracingbutgood
        src/camera.h
        src/Framebuffer.cpp
        src/geometry.h
        src/hittable_list.cpp
        src/lodepng.cpp
        src/main.cpp
        src/meshLoader.h
        src/sphere.cpp
        #    src/triangle.h
        src/common.h
        src/Framebuffer.h
        src/hittable.h
        src/hittable_list.h
        src/lodepng.h
        src/material.h
        src/Ray.h
        src/sphere.h
        src/meshLoader.h
        src/interval.h
        src/aabb.cpp
        src/aabb.h
        src/bvh_node.cpp
        src/bvh_node.h
        src/interval.cpp
        src/texture.h
        src/texture.cpp
        src/image_texture.h
        src/perlin.cpp
        src/perlin.h
        src/quadilateral.cpp
        src/quadilateral.h
        src/lights.h
        src/mesh.h
        src/mesh.cpp
        external/tinyobjloader/tiny_obj_loader.h
        external/tinyobjloader/tiny_obj_loader.cc
        src/objLoader.h
        src/data.h
)
target_link_libraries(raytracingbutgood glm assimp tbb)
target_include_directories(raytracingbutgood PRIVATE external/stb_image)
target_include_directories(raytracingbutgood PRIVATE external/tinyobjloader)

install(TARGETS raytracingbutgood RUNTIME DESTINATION bin)
