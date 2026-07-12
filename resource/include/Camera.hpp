
#pragma once

#include "Ray.hpp"
#include <cstddef>
#include <glm/ext/vector_double3.hpp>
#include <glm/geometric.hpp>

namespace rc {

struct Camera {
  Camera(glm::dvec3 orig, double viewPortWidth, double viewPortHeight,
         size_t imageWidth, size_t imageHeight, double focalLength,
         glm::dvec3 up, glm::dvec3 forward, size_t maxDepth_)
      : orig_(orig), maxDepth_(maxDepth_) {
    auto n_forward = glm::normalize(forward);
    auto n_up = glm::normalize(up);
    u = (viewPortWidth / imageWidth) * glm::cross(n_forward, n_up);
    v = -(viewPortHeight / imageHeight) * n_up;
    higherLeftCorner = focalLength * n_forward +
                       0.5 * viewPortWidth * glm::cross(n_up, n_forward) +
                       0.5 * n_up * viewPortHeight + 0.5 * u + 0.5 * v;
  }

  Ray get(size_t x, size_t y) const {
    return {orig_, higherLeftCorner + static_cast<double>(x) * u +
                       static_cast<double>(y) * v};
  }

  Ray operator()(size_t x, size_t y) const { return get(x, y); }

  glm::dvec3 orig_;
  glm::dvec3 u, v;
  glm::dvec3 higherLeftCorner;
  size_t maxDepth_;
};

} // namespace rc
