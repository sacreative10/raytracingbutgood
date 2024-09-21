//
// Created by sacre on 19/08/2024.
//

#ifndef OCTREE_H
#define OCTREE_H
#include "aabb.h"
#include "triangle.h"


class Octree
{
public:
    Octree(const aabb& boundingBox, int maxDepth = 20, int maxTrianglesPerNode = 10)
        : bbox(boundingBox), depth(0), maxDepth(maxDepth), maxTrianglesPerNode(maxTrianglesPerNode)
    {
    }

    void build(const std::vector<std::shared_ptr<Triangle>>& triangles, int depth = 0);

    bool hit(const Ray& r, float t_min, float t_max, hitrecord& rec) const;

private:
    void subdivide();

private:
    aabb bbox;
    int depth;
    int maxDepth;
    int maxTrianglesPerNode;

    std::vector<std::shared_ptr<Triangle>> triangles;
    std::unique_ptr<Octree> children[8];
};


#endif //OCTREE_H
