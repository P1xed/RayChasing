#pragma once

#include <cstdint>
#include <glm/ext/vector_double3.hpp>
#include <utility>
#include <vector>

#include "Hit.hpp"
#include "Ray.hpp"

namespace rc {

enum class PrimitiveType : uint8_t { SmoothTri, FlatTri, Sph };

struct taggedIdx {
  taggedIdx(uint64_t value, PrimitiveType type) : value_(value), type_(type) {}
  uint64_t value_ : 56;
  PrimitiveType type_ : 8;
};

class Scene;
class AABB;
class PrimitiveView {
public:
  std::vector<taggedIdx> primitiveIndexs_; // TODO maybe later indexs can be
                                           // strore sperately by tmeplate

  PrimitiveView() = default;
  PrimitiveView(const PrimitiveView &) = delete;
  PrimitiveView &operator=(const PrimitiveView &) = default;
  PrimitiveView(PrimitiveView &&) = default;
  PrimitiveView &operator=(PrimitiveView &&) = default;

  void intersect(const Scene &sc, const Ray &r, HitInfo *h) const;
  void getCentroids(const Scene &sc, std::vector<glm::dvec3> *out) const;
  void getAABBs(const Scene &sc, std::vector<AABB> *out) const;
  AABB getAABB(const Scene &sc) const;
  size_t size() const;
};

template <class SceneT, class F>
decltype(auto) visitPrimitive(const SceneT &sc, const taggedIdx &p, F &&f) {
  switch (p.type_) {
  case PrimitiveType::SmoothTri:
    return std::forward<F>(f)(sc.smoothTris_[p.value_]);
  case PrimitiveType::FlatTri:
    return std::forward<F>(f)(sc.flatTris_[p.value_]);
  case PrimitiveType::Sph:
    return std::forward<F>(f)(sc.sphs_[p.value_]);
  }
  std::unreachable();
}

} // namespace rc
