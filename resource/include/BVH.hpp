
#pragma once
#include "PrimitiveView.hpp"
#include <cstddef>
#include <cstdint>
#include <glm/common.hpp>
#include <glm/ext/vector_double3.hpp>
#include <limits>
#include <sys/types.h>

namespace rc {

constexpr size_t BVH_LEAF_FLAG = std::numeric_limits<uint32_t>::max();

class Scene;
struct AABB {
  glm::dvec3 min_ = glm::dvec3(+std::numeric_limits<double>::max());
  glm::dvec3 max_ = glm::dvec3(-std::numeric_limits<double>::max());

  void merge(const AABB &o);
  AABB minClip(uint8_t axis, double x);
  AABB maxClip(uint8_t axis, double x);
  double surfaceArea() const;
  bool intersect(const Ray &r, double tMin, double tMax, double *t) const;
};

struct BVHRef {
  taggedIdx idx_;
  AABB box_;
};

struct BVHLeaf {
  PrimitiveView primitives_;
};

struct BVHNode {
  uint32_t left_, right_;
  AABB box_;

  static size_t build(std::vector<BVHRef> refs, Scene *sc);
  static size_t build(std::vector<BVHRef> refs, AABB box, Scene *sc);
  static size_t newNode(Scene *sc);
  static size_t newLeaf(const std::vector<BVHRef> &refs, Scene *sc);
};

} // namespace rc
