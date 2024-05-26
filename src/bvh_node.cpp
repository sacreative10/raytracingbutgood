//
// Created by sohaibalam on 26/05/24.
//

#include "bvh_node.h"
bvh_node::bvh_node(std::vector<shared_ptr<Hittable>> &objects, size_t start,
                   size_t end) {
  bbox = aabb::empty;
  for (size_t object_index = start; object_index < end; object_index++) {
    bbox = aabb(bbox, objects[object_index]->bounding_box());
  }

  int axis = bbox.longest_axis();

  auto comparator = (axis == 0)   ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;

  size_t object_span = end - start;

  if (object_span == 1) {
    left = right = objects[start];
  } else if (object_span == 2) {
    left = objects[start];
    right = objects[start + 1];
  } else {
    std::sort(objects.begin() + start, objects.begin() + end, comparator);

    auto mid = start + object_span / 2;
    left = make_shared<bvh_node>(objects, start, mid);
    right = make_shared<bvh_node>(objects, mid, end);
  }
}
