
#pragma once

#include "Buffer.hpp"
#include "Pixel.hpp"
#include <cstddef>

namespace rc {

template <size_t Width, size_t Height> struct Film {
  Buffer<Pixel, Width * Height> data_;

  Pixel &operator[](size_t i) { return data_[i]; }
  const Pixel &operator[](size_t i) const { return data_[i]; }
};

} // namespace rc
