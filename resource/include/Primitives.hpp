
#pragma once

#include "Hit.hpp"
#include "Primitive.hpp"
#include "Ray.hpp"
#include <cstddef>
#include <vector>
namespace rc {

class Primitives {
  std::vector<SmoothTri> *smoothTrisIndexsPtr_;
  std::vector<FlatTri> *flatTrisIndexsPtr_;
  std::vector<Sph> *sphIndexsPtr_;

public:
  std::vector<size_t> smoothTrisIndexs_;
  std::vector<size_t> flatTrisIndexs_;
  std::vector<size_t> sphIndexs_;

  Primitives(const Primitives &) = delete;
  Primitives &operator=(const Primitives &) = delete;
  Primitives(Primitives &&) = delete;
  Primitives &operator=(Primitives &&) = delete;
  Primitives(std::vector<SmoothTri> *smoothTrisIndexsPtr,
             std::vector<FlatTri> *flatTrisIndexsPtr,
             std::vector<Sph> *sphIndexsPtr)
      : smoothTrisIndexsPtr_(smoothTrisIndexsPtr),
        flatTrisIndexsPtr_(flatTrisIndexsPtr), sphIndexsPtr_(sphIndexsPtr) {}

  void intersect(const Ray &r, HitInfo *h) const {
    for (size_t index : smoothTrisIndexs_)
      (*smoothTrisIndexsPtr_)[index].intersect(r, h);
    for (size_t index : flatTrisIndexs_)
      (*flatTrisIndexsPtr_)[index].intersect(r, h);
    for (size_t index : sphIndexs_)
      (*sphIndexsPtr_)[index].intersect(r, h);
  }
};

} // namespace rc
