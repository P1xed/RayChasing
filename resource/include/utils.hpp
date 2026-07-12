
#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>

namespace rc {

template <std::size_t I, typename F> void staticFor(F &&f) {
  []<std::size_t... Is>(F &&f, std::index_sequence<Is...>) {
    (f(std::integral_constant<std::size_t, Is>{}), ...);
  }(std::forward<F>(f), std::make_index_sequence<I>{});
}

template <std::size_t I, typename F> void parallelFor(F &&f) {}
// TODO:
// implement this by thread pool

} // namespace rc
