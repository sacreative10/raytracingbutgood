// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2022 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR
// LicenseRef-KDE-Accepted-GPL

#include "hittable_list.h"

hittable_list::hittable_list(std::shared_ptr<Hittable> object) { add(object); }

void hittable_list::clear() { objects.clear(); }

void hittable_list::add(std::shared_ptr<Hittable> object) {
  objects.push_back(object);
  bbox = aabb(bbox, object->bounding_box());
}

bool hittable_list::hit(const Ray &r, float t_min, float t_max,
                        hitrecord &rec) const {
  hitrecord temp_record;
  bool hit_anything;
  float closest_so_far = t_max;

  for (const auto &object : objects) {
    if (object->hit(r, t_min, closest_so_far, temp_record)) {
      hit_anything = true;
      closest_so_far = temp_record.t;
      rec = temp_record;
    }
  }

  return hit_anything;
}
