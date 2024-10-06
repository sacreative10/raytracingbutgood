#pragma once
#include <iostream>
#include <utility>

#include "bvh_node.h"
#include "common.h"
#include "glm/geometric.hpp"
#include "hittable.h"
#include "quadilateral.h"
#include "Octree.h"
#include "triangle.h"


class Mesh : public Hittable
{
public:
    Mesh(const std::vector<glm::vec3> vertices, const std::vector<int> indices, std::vector<glm::vec3>& normals,
         std::shared_ptr<material> mat,
         Transform modelMatrix = Transform())
    {
        for (int i = 0; i < indices.size(); i += 3)
        {
            int index = indices[i];
            Vec3 v0 = vertices[index];
            Vec3 v1 = vertices[index + 1];
            Vec3 v2 = vertices[index + 2];

            v0 = glm::vec3(modelMatrix * glm::vec4(v0, 1.0f));
            v1 = glm::vec3(modelMatrix * glm::vec4(v1, 1.0f));
            v2 = glm::vec3(modelMatrix * glm::vec4(v2, 1.0f));

            triangles.push_back(std::make_shared<Triangle>(v0, v1, v2, mat, normals[i / 3]));
            list.add(triangles.back());
        }

        calculate_bounding_box(vertices, modelMatrix);

        bvh = std::make_unique<bvh_node>(list.objects, 0, list.objects.size());
    }

    Mesh(const std::string& filename, std::shared_ptr<material> mat, Transform modelMatrix = Transform(), bool useOctree = true);

    bool hit(const Ray& r, float t_min, float t_max, hitrecord& rec) const override;

    aabb bounding_box() const override
    {
        return bbox;
    }

private:
    std::vector<std::shared_ptr<Triangle>> triangles;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    hittable_list list;
    std::unique_ptr<bvh_node> bvh;
    bool useOctree;
    Octree* octree;


    aabb bbox;

    void calculate_bounding_box(const std::vector<glm::vec3>& vertices, Transform modelMatrix = Transform());
};
