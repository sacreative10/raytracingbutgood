cmake_minimum_required(VERSION 3.0)

project(raytracingbutgood)

file(GLOB SOURCES src/*.cpp)

add_subdirectory(external/glm)
add_executable(raytracingbutgood ${SOURCES})
target_link_libraries(raytracingbutgood glm)

install(TARGETS raytracingbutgood RUNTIME DESTINATION bin)
