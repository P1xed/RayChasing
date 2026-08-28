
#pragma once
#include <atomic>
#include <cstddef>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace rc {

template <size_t I, typename F> void static_for(F &&f) {
  []<size_t... Is>(F &&f, std::index_sequence<Is...>) {
    (f(std::integral_constant<size_t, Is>{}), ...);
  }(std::forward<F>(f), std::make_index_sequence<I>{});
}

template <size_t I, typename F> void parallelFor(F &&f) {
  if constexpr (I == 0)
    return;
  size_t n = std::thread::hardware_concurrency();
  std::atomic<size_t> next = 0;
  std::vector<std::thread> threads;
  threads.reserve(n);
  for (size_t i = 0; i < n; ++i)
    threads.emplace_back([&]() {
      while (true) {
        size_t idx = next.fetch_add(1, std::memory_order::relaxed);
        if (idx >= I)
          return;
        f(idx);
      }
    });
  for (std::thread &t : threads)
    t.join();
}

} // namespace rc
