
#pragma once
#include <glm/ext/vector_double3.hpp>
#include <limits>
namespace rc {

struct HitInfo {
  glm::dvec3 point_ = {};
  glm::dvec3 normal_ = {};
  double tMax_ = std::numeric_limits<double>::infinity();
  double tMin_ = 0;
  bool miss() const { return tMax_ == std::numeric_limits<double>::infinity(); }
};
} // namespace rc
