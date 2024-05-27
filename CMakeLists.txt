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
        #src/mesh.cpp
        src/meshLoader.h
        src/sphere.cpp
        src/triangle.h
        src/common.h
        src/Framebuffer.h
        src/hittable.h
        src/hittable_list.h
        src/lodepng.h
        src/material.h
        #src/mesh.h
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
)
target_link_libraries(raytracingbutgood glm assimp tbb)

install(TARGETS raytracingbutgood RUNTIME DESTINATION bin)
