//
// Created by sohaibalam on 26/05/24.
//

#ifndef RAYTRACINGBUTGOOD_BVH_NODE_H
#define RAYTRACINGBUTGOOD_BVH_NODE_H

#include <algorithm>

#include "hittable_list.h"
class bvh_node : public Hittable {

public:
  bvh_node(hittable_list &list)
      : bvh_node(list.objects, 0, list.objects.size()) {}

  bool hit(const Ray &r, float t_min, float t_max,
           hitrecord &rec) const override {
    if (!bbox.hit(r, interval(t_min, t_max)))
      return false;

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

    return hit_left || hit_right;
  }

  aabb bounding_box() const { return bbox; }

  bvh_node(std::vector<shared_ptr<Hittable>> &objects, size_t start,
           size_t end);

private:
  std::shared_ptr<Hittable> left;
  std::shared_ptr<Hittable> right;
  aabb bbox;

  static bool box_compare(const shared_ptr<Hittable> a,
                          const shared_ptr<Hittable> b, int axis) {
    auto a_axis_interval = a->bounding_box().axis_interval(axis);
    auto b_axis_interval = b->bounding_box().axis_interval(axis);
    return a_axis_interval.min < b_axis_interval.min;
  }

  static bool box_x_compare(const shared_ptr<Hittable> a,
                            const shared_ptr<Hittable> b) {
    return box_compare(a, b, 0);
  }
  static bool box_y_compare(const shared_ptr<Hittable> a,
                            const shared_ptr<Hittable> b) {
    return box_compare(a, b, 1);
  }
  static bool box_z_compare(const shared_ptr<Hittable> a,
                            const shared_ptr<Hittable> b) {
    return box_compare(a, b, 2);
  }
};

#endif // RAYTRACINGBUTGOOD_BVH_NODE_H
