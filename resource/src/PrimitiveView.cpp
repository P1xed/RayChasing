
#include "PrimitiveView.hpp"
#include "BVH.hpp"
#include "Scene.hpp"
#include <glm/common.hpp>

namespace rc {

void PrimitiveView::getCentroids(const Scene &sc,
                                 std::vector<glm::dvec3> *out) const {
  out->clear();
  out->reserve(primitiveIndexs_.size());
  for (taggedIdx p : primitiveIndexs_)
    out->push_back(visitPrimitive(
        sc, p, [](const auto &prim) { return prim.getCentroid(); }));
}
void PrimitiveView::getAABBs(const Scene &sc, std::vector<AABB> *out) const {
  out->clear();
  out->reserve(primitiveIndexs_.size());
  for (taggedIdx p : primitiveIndexs_)
    out->push_back(
        visitPrimitive(sc, p, [](const auto &prim) { return prim.getAABB(); }));
}

void PrimitiveView::intersect(const Scene &sc, const Ray &r, HitInfo *h) const {
  for (taggedIdx p : primitiveIndexs_)
    visitPrimitive(sc, p, [&r, h](const auto &prim) { prim.intersect(r, h); });
}

size_t PrimitiveView::size() const { return primitiveIndexs_.size(); }

AABB PrimitiveView::getAABB(const Scene &sc) const {
  AABB box{glm::dvec3(INFINITY), glm::dvec3(-INFINITY)};
  for (taggedIdx objView : primitiveIndexs_) {
    AABB b = visitPrimitive(sc, objView,
                            [](const auto &prim) { return prim.getAABB(); });
    box.min_ = glm::min(box.min_, b.min_);
    box.max_ = glm::max(box.max_, b.max_);
  }
  return box;
}

} // namespace rc
