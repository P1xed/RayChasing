
#pragma once

#include <glm/ext/vector_double3.hpp>
namespace rc {

struct Ray {
  glm::dvec3 orig_, dir_, invDir_;
  glm::dvec3 at(double t) const { return orig_ + t * dir_; }
};

} // namespace rc
