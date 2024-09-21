#pragma once

#include <glm/glm.hpp>

const float EPSILON = 0.0001f;

using Vec3 = glm::vec3;
using Vec2 = glm::vec2;
using Point3 = glm::vec3;
using point2 = glm::vec2;

using Transform = glm::mat4;

inline Vec3 applyTransform(const Transform &t, const Vec3 &v) {
  return (t * glm::vec4(v, 1)).xyz();
}