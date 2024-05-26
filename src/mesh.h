
#pragma once

#include "geometry.h"
#include <iostream>

#include "Ray.h"
#include "hittable.h"

struct Vertex {
  Vec3 position;
  Vec3 normal;
  Vec2 texCoords;
  Vertex(Vec3 pos, Vec3 norm, Vec2 texCoords)
      : position(pos), normal(norm), texCoords(texCoords) {}
};

class Triangle : public Hittable {
public:
  Triangle(std::vector<Vertex> *vertices, int v0, int v1, int v2,
           shared_ptr<material> mat = nullptr)
      : vertices(vertices), v0(v0), v1(v1), v2(v2), mat(mat) {}

  virtual bool hit(const Ray &ray, float tMin, float tMax,
                   hitrecord &record) const override;

  Vec3 getNormal() const;

public:
  void setvertexPointer(std::vector<Vertex> *vertices) {
    this->vertices = vertices;
  }

private:
  std::vector<Vertex> *vertices;
  int v0, v1, v2;
  shared_ptr<material> mat;
};

#include "material.h"
class Mesh : public Hittable {

public:
  Mesh(std::vector<Vertex> vertices, std::vector<int> indices,
       shared_ptr<material> mat = nullptr)
      : vertices(vertices) {
    if (mat == nullptr) {
      this->m_mat = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
    }

    for (int i = 0; i < indices.size(); i += 3) {
      std::cout << "Adding triangle: " << indices[i] << " " << indices[i + 1]
                << " " << indices[i + 2] << std::endl;
      triangles.push_back(std::make_shared<Triangle>(
          &this->vertices, indices[i], indices[i + 1], indices[i + 2],
          this->m_mat));
    }
  }

  virtual bool hit(const Ray &ray, float tMin, float tMax,
                   hitrecord &record) const override;

private:
  std::vector<Vertex> vertices;
  std::vector<std::shared_ptr<Triangle>> triangles;
  shared_ptr<material> m_mat;
};