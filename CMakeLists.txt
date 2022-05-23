cmake_minimum_required(VERSION 3.0)

project(raytracingbutgood)

add_executable(raytracingbutgood main.cpp)

install(TARGETS raytracingbutgood RUNTIME DESTINATION bin)
