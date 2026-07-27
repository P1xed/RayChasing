
#pragma once
#include <atomic>
#include <cstddef>
#include <functional>
#include <thread>
#include <vector>
namespace rc {

class StaticThreadPool {
  std::vector<std::thread> threads_;
  std::vector<std::function<void()>> tasks_;
  std::atomic<size_t> taskIndex_ = 0;

public:
  void addTask(std::function<void()> task) { tasks_.push_back(task); }

  void runTasks() {

    auto n = std::thread::hardware_concurrency();

    for (size_t i = 0; i != n; i++) {
      threads_.emplace_back(std::thread([this]() {
        while (true) {
          auto index = taskIndex_.fetch_add(1, std::memory_order::relaxed);
          if (index >= tasks_.size())
            return;
          tasks_[index]();
        }
      }));
    }

    for (auto &t : threads_)
      t.join();
  }

  void clear() {
    tasks_.clear();
    threads_.clear();
    taskIndex_ = 0;
  }
};

class DynamicThreadPool {};

} // namespace rc
