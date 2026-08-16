
#pragma once

#include <sys/types.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <glm/common.hpp>
#include <glm/ext/vector_double3.hpp>

#include "PrimitiveView.hpp"

namespace rc {

class Scene;
struct AABB {
  glm::dvec3 min_;
  glm::dvec3 max_;

  void merge(const AABB &o) {
    min_ = glm::min(min_, o.min_);
    max_ = glm::max(max_, o.max_);
  }

  AABB minClip(uint8_t axis, double x) {
    if (min_[axis] < x)
      min_[axis] = x;
    return *this;
  }

  AABB maxClip(uint8_t axis, double x) {
    if (max_[axis] > x)
      max_[axis] = x;
    return *this;
  }

  double surfaceArea() const {
    glm::dvec3 d = max_ - min_;
    return 2.0 * (d.x * d.y + d.y * d.z + d.z * d.x);
  }

bool intersect(const Ray &r, double tMin, double tMax, double *t) const {
    double tmin = tMin, tmax = tMax;
    for (size_t i = 0; i < 3; i++) {
      double t0 = (min_[i] - r.orig_[i]) * r.invDir_[i];
      double t1 = (max_[i] - r.orig_[i]) * r.invDir_[i];
      if (r.invDir_[i] < 0)
        std::swap(t0, t1);
      tmin = std::max(tmin, t0);
      tmax = std::min(tmax, t1);
      if (tmin > tmax)
        return false;
    }
    *t = tmin;
    return true;
  }
};

struct BVHNode {
  PrimitiveView primitives_;
  size_t left_ = SIZE_MAX;
  size_t right_ = SIZE_MAX;
  AABB box_;

  static size_t build(const PrimitiveView &primitives, AABB box, Scene *sc);
};

} // namespace rc
