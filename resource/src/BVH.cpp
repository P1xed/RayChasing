
#include "BVH.hpp"
#include <cstddef>
#include <cstdint>
#include <glm/ext/vector_double3.hpp>
#include <vector>
#include "PrimitiveView.hpp"
#include "Scene.hpp"
#include "utils.hpp"

size_t rc::BVHNode::build(const PrimitiveView &primitives, AABB box,
                          Scene *sc) {
  const size_t idx = sc->BVHNodes_.size();
  sc->BVHNodes_.emplace_back();
  sc->BVHNodes_[idx].box_ = box;
  sc->BVHNodes_[idx].box_.padEpsilon();
  sc->BVHNodes_[idx].primitives_ = primitives;
  sc->BVHNodes_[idx].left_= SIZE_MAX;

  if (primitives.size() > 8) {
    std::vector<glm::dvec3> centroids;
    primitives.getCentroids(*sc, &centroids);

    glm::dvec3 cMin = centroids[0];
    glm::dvec3 cMax = centroids[0];
    for (glm::dvec3 &c : centroids)
      rc::staticFor<3>([&](size_t i) {
        if (c[i] < cMin[i])
          cMin[i] = c[i];
        if (c[i] > cMax[i])
          cMax[i] = c[i];
      });

    constexpr uint8_t X = 0, Y = 1, Z = 2;
    glm::dvec3 extent = cMax - cMin;
    int axis = X;
    if (extent[Y] > extent[axis])
      axis = Y;
    if (extent[Z] > extent[axis])
      axis = Z;
    double cMid = 0.5 * (cMin[axis] + cMax[axis]);

    PrimitiveView left, right;
    for (auto &obj : primitives.primitiveIndexs_) {
      switch (obj.type_) {
      case rc::PrimitiveType::SmoothTri: {
        double min = sc->smoothTris_[obj.value_].getAABB().min_[axis];
        double max = sc->smoothTris_[obj.value_].getAABB().max_[axis];
        if (min <= cMid)
          left.primitiveIndexs_.push_back(obj);
        if (max > cMid)
          right.primitiveIndexs_.push_back(obj);
        break;
      }
      case rc::PrimitiveType::FlatTri: {
        double min = sc->flatTris_[obj.value_].getAABB().min_[axis];
        double max = sc->flatTris_[obj.value_].getAABB().max_[axis];
        if (min <= cMid)
          left.primitiveIndexs_.push_back(obj);
        if (max > cMid)
          right.primitiveIndexs_.push_back(obj);
        break;
      }
      case rc::PrimitiveType::Sph: {
        double min = sc->sphs_[obj.value_].getAABB().min_[axis];
        double max = sc->sphs_[obj.value_].getAABB().max_[axis];
        if (min <= cMid)
          left.primitiveIndexs_.push_back(obj);
        if (max > cMid)
          right.primitiveIndexs_.push_back(obj);
        break;
      }
      }
    }

    if (left.size() != primitives.size() && !left.primitiveIndexs_.empty())
      if (right.size() != primitives.size() &&
          !right.primitiveIndexs_.empty()) {
        sc->BVHNodes_[idx].left_ =
            build(left, left.getAABB(*sc).maxClip(axis, cMid), sc);
        sc->BVHNodes_[idx].right_ =
            build(right, right.getAABB(*sc).minClip(axis, cMid), sc);
      }
  }

  return idx;
}
