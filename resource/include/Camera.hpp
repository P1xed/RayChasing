
#pragma once

#include "Ray.hpp"
#include <cstddef>
#include <glm/ext/vector_double3.hpp>
#include <glm/geometric.hpp>

namespace rc {

struct Camera {
  Camera(glm::dvec3 orig, double viewPortWidth, double viewPortHeight,
         size_t imageWidth, size_t imageHeight, double focalLength,
         glm::dvec3 up, glm::dvec3 forward, size_t maxDepth)
      : orig_(orig), maxDepth_(maxDepth) {
    glm::dvec3 n_forward = glm::normalize(forward);
    glm::dvec3 n_up = glm::normalize(up);
    u_ = (viewPortWidth / imageWidth) * glm::cross(n_forward, n_up);
    v_ = -(viewPortHeight / imageHeight) * n_up;
    higherLeftCorner_ = focalLength * n_forward +
                        0.5 * viewPortWidth * glm::cross(n_up, n_forward) +
                        0.5 * n_up * viewPortHeight + 0.5 * u_ + 0.5 * v_;
  }

  Ray get(size_t x, size_t y) const {
    // TODO: maybe require optimization
    return {orig_, higherLeftCorner_ + static_cast<double>(x) * u_ +
                       static_cast<double>(y) * v_};
  }

  Ray operator()(size_t x, size_t y) const { return get(x, y); }

  glm::dvec3 orig_;
  glm::dvec3 u_, v_;
  glm::dvec3 higherLeftCorner_;
  size_t maxDepth_;
};

} // namespace rc
