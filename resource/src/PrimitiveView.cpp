#include "PrimitiveView.hpp"

#include <glm/common.hpp>

#include "BVH.hpp"
#include "Scene.hpp"

namespace rc {

void PrimitiveView::getCentroids(const Scene &sc,
                                 std::vector<glm::dvec3> *out) const {
  out->clear();
  out->reserve(primitiveIndexs_.size());
  for (taggedIdx p : primitiveIndexs_)
    switch (p.type_) {
    case PrimitiveType::SmoothTri:
      out->push_back(sc.smoothTris_[p.value_].getCentroid());
      break;
    case PrimitiveType::FlatTri:
      out->push_back(sc.flatTris_[p.value_].getCentroid());
      break;
    case PrimitiveType::Sph:
      out->push_back(sc.sphs_[p.value_].getCentroid());
      break;
    }
}

void PrimitiveView::intersect(const Scene &sc, const Ray &r, HitInfo *h) const {
  for (taggedIdx p : primitiveIndexs_)
    switch (p.type_) {
    case PrimitiveType::SmoothTri:
      sc.smoothTris_[p.value_].intersect(r, h);
      break;
    case PrimitiveType::FlatTri:
      sc.flatTris_[p.value_].intersect(r, h);
      break;
    case PrimitiveType::Sph:
      sc.sphs_[p.value_].intersect(r, h);
      break;
    }
}

size_t PrimitiveView::size() const { return primitiveIndexs_.size(); }

AABB PrimitiveView::getAABB(const Scene &sc) const {
  AABB box{glm::dvec3(INFINITY), glm::dvec3(-INFINITY)};
  for (taggedIdx objView : primitiveIndexs_) {
    AABB b;
    switch (objView.type_) {
    case PrimitiveType::SmoothTri:
      b = sc.smoothTris_[objView.value_].getAABB();
      break;
    case PrimitiveType::FlatTri:
      b = sc.flatTris_[objView.value_].getAABB();
      break;
    case PrimitiveType::Sph:
      b = sc.sphs_[objView.value_].getAABB();
      break;
    }
    box.min_ = glm::min(box.min_, b.min_);
    box.max_ = glm::max(box.max_, b.max_);
  }
  return box;
}

} // namespace rc
