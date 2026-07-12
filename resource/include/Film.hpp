
#pragma once

#include "Buffer.hpp"
#include "Pixel.hpp"
#include <cstddef>

namespace rc {

template <std::size_t Width, std::size_t Height> struct Film {
  Buffer<Pixel, Width * Height> data_;

  Pixel &operator[](std::size_t i) { return data_[i]; }
  const Pixel &operator[](std::size_t i) const { return data_[i]; }
};

} // namespace rc
