
#pragma once
#include "BVH.hpp"
#include "Hit.hpp"
#include "Ray.hpp"
#include <array>
#include <glm/ext/vector_double3.hpp>

namespace rc {

struct SmoothTri {
  std::array<glm::dvec3, 3> pos_;
  std::array<glm::dvec3, 3> nor_;

  void intersect(const Ray &r, HitInfo *h) const;
  AABB getAABB() const;
  glm::dvec3 getCentroid() const;
};

struct FlatTri {
  std::array<glm::dvec3, 3> pos_;
  glm::dvec3 nor_;

  void intersect(const Ray &r, HitInfo *h) const;
  AABB getAABB() const;
  glm::dvec3 getCentroid() const;
};

struct Sph {
  glm::dvec3 cen_;
  double rad_;

  void intersect(const Ray &r, HitInfo *h) const;
  AABB getAABB() const;
  glm::dvec3 getCentroid() const;
};

} // namespace rc
