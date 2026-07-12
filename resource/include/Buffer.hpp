
#pragma once

#include <cstddef>
#include <memory>
namespace rc {

template <typename T, std::size_t N> struct Buffer {
  std::unique_ptr<T[]> data_ = std::make_unique<T[]>(N);
  T &operator[](std::size_t i) { return data_[i]; }
  const T &operator[](std::size_t i) const { return data_[i]; }
};
} // namespace rc
