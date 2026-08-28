#include "BVH.hpp"
#include "PrimitiveView.hpp"
#include "Scene.hpp"
#include "utils.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <glm/common.hpp>
#include <glm/ext/vector_double3.hpp>
#include <limits>
#include <sys/types.h>
#include <utility>
#include <vector>

void rc::AABB::merge(const AABB &o) {
  min_ = glm::min(min_, o.min_);
  max_ = glm::max(max_, o.max_);
}

rc::AABB rc::AABB::minClip(uint8_t axis, double x) {
  if (min_[axis] < x)
    min_[axis] = x;
  return *this;
}

rc::AABB rc::AABB::maxClip(uint8_t axis, double x) {
  if (max_[axis] > x)
    max_[axis] = x;
  return *this;
}

double rc::AABB::surfaceArea() const {
  glm::dvec3 d = max_ - min_;
  return 2.0 * (d.x * d.y + d.y * d.z + d.z * d.x);
}

bool rc::AABB::intersect(const Ray &r, double tMin, double tMax,
                         double *t) const {
  double t0 = tMin, t1 = tMax;
  rc::static_for<3>([&](auto i) {
    double invD = r.invDir_[i];
    double a = (min_[i] - r.orig_[i]) * invD;
    double b = (max_[i] - r.orig_[i]) * invD;
    t0 = std::max(t0, std::min(a, b));
    t1 = std::min(t1, std::max(a, b));
  });
  if (t0 > t1)
    return false;
  *t = t0;
  return true;
}

size_t rc::BVHNode::build(std::vector<BVHRef> refs, Scene *sc) {
  if (refs.size() < 8)
    return newLeaf(refs, sc);

  AABB box;
  for (const auto &r : refs)
    box.merge(r.box_);
  return build(std::move(refs), box, sc);
}

size_t rc::BVHNode::build(std::vector<BVHRef> refs, AABB box, Scene *sc) {
  if (refs.size() < 8)
    return newLeaf(refs, sc);

  static constexpr uint8_t binNum = 16;

  int bestAxis = 0;
  double bestExtent = 0.;
  double axisMin[3], axisExtent[3];
  rc::static_for<3>([&](auto axis) {
    double minC = +std::numeric_limits<double>::max();
    double maxC = -std::numeric_limits<double>::max();
    for (const auto &r : refs) {
      double c = 0.5 * (r.box_.min_[axis] + r.box_.max_[axis]);
      minC = std::min(minC, c);
      maxC = std::max(maxC, c);
    }
    double extent = maxC - minC;
    axisMin[axis] = minC;
    axisExtent[axis] = extent;
    if (extent > bestExtent) {
      bestExtent = extent;
      bestAxis = axis;
    }
  });

  double minC = axisMin[bestAxis];
  double binStep = axisExtent[bestAxis] / binNum;

  double parentSAH = box.surfaceArea() * refs.size();
  double bestSAH = parentSAH;
  double bestSplitSurface = minC + axisExtent[bestAxis] * .5;

  std::array<std::pair<uint32_t, AABB>, binNum> bins;

  for (const auto &ref : refs) {
    visitPrimitive(*sc, ref.idx_, [&](const auto &prim) {
      int binIdx = static_cast<int>(
          (prim.getCentroid()[bestAxis] - axisMin[bestAxis]) / binStep);
      binIdx = static_cast<int>(binNum) - 1;
      auto &[count, box] = bins[binIdx];
      count++;
      box.merge(ref.box_);
    });
  }

  for (size_t i = 0; i != binNum - 1; i++) {
    double SplitSurface = axisMin[bestAxis] + (i + 1) * binStep;

    uint32_t leftCount = 0, rightCount = 0;
    AABB leftBox, rightBox;

    for (size_t j = 0; j != binNum - 1; j++) {
      auto &[count, box] = bins[j];
      if (j <= i) {
        leftCount += count;
        leftBox.merge(box);
      } else {
        rightCount += count;
        rightBox.merge(box);
      }
    }// TODO: fix this: O(N²)

    double sah =
        leftBox.surfaceArea() * leftCount + rightBox.surfaceArea() * rightCount;
    if (sah < bestSAH) {
      bestSAH = sah;
      bestSplitSurface = SplitSurface;
    }
  }
  std::vector<BVHRef> leftRefs, rightRefs;
  for (const auto &r : refs) {
    const bool inLeft = r.box_.min_[bestAxis] <= bestSplitSurface;
    const bool inRight = r.box_.max_[bestAxis] >= bestSplitSurface;
    if (inLeft) {
      BVHRef lr = r;
      lr.box_.maxClip(bestAxis, bestSplitSurface);
      leftRefs.push_back(lr);
    }
    if (inRight) {
      BVHRef rr = r;
      rr.box_.minClip(bestAxis, bestSplitSurface);
      rightRefs.push_back(rr);
    }
  }

  if (leftRefs.empty() or rightRefs.empty() or leftRefs.size() == refs.size() or
      rightRefs.size() == refs.size())
    return newLeaf(refs, sc);

  AABB leftBox;
  AABB rightBox;
  for (const auto &r : leftRefs)
    leftBox.merge(r.box_);
  for (const auto &r : rightRefs)
    rightBox.merge(r.box_);
  leftBox.maxClip(bestAxis, bestSplitSurface);
  rightBox.minClip(bestAxis, bestSplitSurface);

  const size_t idx = newNode(sc);
  sc->BVHNodes_[idx].box_ = box;

  size_t leftIdx = build(std::move(leftRefs), leftBox, sc);
  size_t rightIdx = build(std::move(rightRefs), rightBox, sc);

  sc->BVHNodes_[idx].left_ = leftIdx;
  sc->BVHNodes_[idx].right_ = rightIdx;
  return idx;
}

size_t rc::BVHNode::newNode(Scene *sc) {
  const size_t idx = sc->BVHNodes_.size();
  sc->BVHNodes_.emplace_back();
  return idx;
}

size_t rc::BVHNode::newLeaf(const std::vector<BVHRef> &refs, Scene *sc) {
  AABB box;
  for (const auto &r : refs)
    box.merge(r.box_);
  const size_t idx = newNode(sc);
  BVHNode &node = sc->BVHNodes_[idx];
  node.box_ = box;
  BVHLeaf leaf;
  leaf.primitives_.primitiveIndexs_.reserve(refs.size());
  for (const auto &r : refs)
    leaf.primitives_.primitiveIndexs_.push_back(r.idx_);
  const size_t leafIdx = sc->BVHLeaves_.size();
  sc->BVHLeaves_.push_back(std::move(leaf));
  node.left_ = BVH_LEAF_FLAG;
  node.right_ = leafIdx;
  return idx;
}
