
#pragma once

#include <cstddef>
#include <glm/ext/vector_double3.hpp>
namespace rc {

struct Bucket {
  static constexpr size_t BucketNumber = 32;
};

struct AABB {
  glm::dvec3 center_;
  double halfXLength_;
  double halfYLength_;
  double halfZLength_;

  AABB(glm::dvec3 center, double halfXLength, double halfYLength,
       double halfZLength)
      : center_(center), halfXLength_(halfXLength), halfYLength_(halfYLength),
        halfZLength_(halfZLength) {}
};
} // namespace rc
