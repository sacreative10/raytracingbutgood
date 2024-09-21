//
// Created by sacre on 19/08/2024.
//

#include "Octree.h"


void Octree::build(const std::vector<std::shared_ptr<Triangle> > &triangles, int depth) {
    this->depth = depth;

    if (triangles.size() <= maxTrianglesPerNode || depth >= maxDepth) {
        this->triangles = triangles;
        return;
    }

    subdivide();

    std::vector<std::shared_ptr<Triangle> > childrenTriangles[8];
    for (const auto &triangle: triangles) {
        aabb triangleBoundingBox = triangle->bounding_box();
        for (int i = 0; i < 8; i++) {
            if (children[i]->bbox.intersect(triangleBoundingBox)) {
                childrenTriangles[i].push_back(triangle);
            }
        }
    }

    // now recursively build the children
    for (int i = 0; i < 8; i++) {
        children[i]->build(childrenTriangles[i], depth + 1);
    }
}

void Octree::subdivide() {
    glm::vec3 center = bbox.center();
    glm::vec3 halfSize = bbox.half_size();

    children[0] = std::make_unique<Octree>(aabb(bbox.min(), center)); // Bottom-left-front
    children[1] = std::make_unique<Octree>(aabb(glm::vec3(center.x, bbox.min().y, bbox.min().z),
                                                glm::vec3(bbox.max().x, center.y, center.z))); // Bottom-right-front
    children[2] = std::make_unique<Octree>(aabb(glm::vec3(center.x, bbox.min().y, center.z),
                                                glm::vec3(bbox.max().x, center.y, bbox.max().z))); // Bottom-right-back
    children[3] = std::make_unique<Octree>(aabb(glm::vec3(bbox.min().x, bbox.min().y, center.z),
                                                glm::vec3(center.x, center.y, bbox.max().z))); // Bottom-left-back
    children[4] = std::make_unique<Octree>(aabb(glm::vec3(bbox.min().x, center.y, bbox.min().z),
                                                glm::vec3(center.x, bbox.max().y, center.z))); // Top-left-front
    children[5] = std::make_unique<Octree>(aabb(glm::vec3(center.x, center.y, bbox.min().z),
                                                glm::vec3(bbox.max().x, bbox.max().y, center.z))); // Top-right-front
    children[6] = std::make_unique<Octree>(aabb(center, bbox.max())); // Top-right-back
    children[7] = std::make_unique<Octree>(aabb(glm::vec3(bbox.min().x, center.y, center.z),
                                                glm::vec3(center.x, bbox.max().y, bbox.max().z))); // Top-left-back
}

bool Octree::hit(const Ray &r, float t_min, float t_max, hitrecord &rec) const {
    if (!bbox.hit(r, interval(t_min, t_max))) {
        return false;
    }

    bool hit_anything = false;
    float closest_so_far = t_max;

    // leaf node
    if (!children[0]) {
        for (const auto &triangle: triangles) {
            if (triangle->hit(r, t_min, closest_so_far, rec)) {
                hit_anything = true;
                closest_so_far = rec.t;
            }
        }
    } else {
        // internal node
        for (int i = 0; i < 8; i++) {
            if (children[i]->hit(r, t_min, closest_so_far, rec)) {
                hit_anything = true;
                closest_so_far = rec.t;
            }
        }
    }

    return hit_anything;
}

