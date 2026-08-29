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

static rc::AABB mergeRefs(const std::vector<rc::BVHRef> &refs) {
  rc::AABB a, b, c, d;
  size_t i = 0;
  for (; i + 3 < refs.size(); i += 4) {
    a.merge(refs[i].box_);
    b.merge(refs[i + 1].box_);
    c.merge(refs[i + 2].box_);
    d.merge(refs[i + 3].box_);
  }
  for (; i < refs.size(); ++i)
    a.merge(refs[i].box_);
  a.merge(b);
  a.merge(c);
  a.merge(d);
  return a;
}

size_t rc::BVHNode::buildRoot(Scene *sc) {
  std::vector<BVHRef> refs;
  refs.reserve(sc->primitives_.primitiveIndexs_.size());
  for (const auto &primitive : sc->primitives_.primitiveIndexs_) {
    visitPrimitive(*sc, primitive, [&](auto prim) {
      refs.push_back(BVHRef{primitive, prim.getAABB()});
    });
  }
  if (refs.size() < 8)
    return newLeaf(refs, sc);

  AABB box = mergeRefs(refs);
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

  double binStep = axisExtent[bestAxis] / binNum;

  double parentSAH = box.surfaceArea() * refs.size();
  double bestSAH = parentSAH;
  double bestSplitSurface = axisMin[bestAxis] + axisExtent[bestAxis] * .5;

  std::array<std::pair<uint32_t, AABB>, binNum> bins;

  for (const auto &ref : refs) {
    double c = 0.5 * (ref.box_.min_[bestAxis] + ref.box_.max_[bestAxis]);
    int binIdx = static_cast<int>((c - axisMin[bestAxis]) / binStep);
    binIdx = std::clamp(binIdx, 0, static_cast<int>(binNum) - 1);
    auto &[count, box] = bins[binIdx];
    count++;
    box.merge(ref.box_);
  }

  std::array<std::pair<uint32_t, AABB>, binNum> leftAccum;
  std::array<std::pair<uint32_t, AABB>, binNum> rightAccum;

  {
    auto &[refCount, refBox] = bins[0];
    leftAccum[0] = std::make_pair(refCount, refBox);
    for (size_t i = 1; i < binNum; i++) {
      auto &[refCount, refBox] = bins[i];
      auto &[count, box] = leftAccum[i];
      auto &[prevCount, prevBox] = leftAccum[i - 1];
      count = prevCount + refCount;
      box.merge(prevBox);
      box.merge(refBox);
    }
  }

  {
    auto &[refCount, refBox] = bins[binNum - 1];
    rightAccum[binNum - 1] = std::make_pair(refCount, refBox);
    for (size_t i = binNum - 1; i-- > 0;) {
      auto &[refCount, refBox] = bins[i];
      auto &[count, box] = rightAccum[i];
      auto &[nextCount, nextBox] = rightAccum[i + 1];
      count = nextCount + refCount;
      box.merge(nextBox);
      box.merge(refBox);
    }
  }
  for (size_t i = 0; i != binNum - 1; i++) {
    double SplitSurface = axisMin[bestAxis] + (i + 1) * binStep;

    uint32_t leftCount = 0, rightCount = 0;
    AABB leftBox, rightBox;

    auto &[laCount, laBox] = leftAccum[i];
    auto &[raCount, raBox] = rightAccum[i + 1];

    leftCount = laCount;
    leftBox = laBox;
    leftBox.maxClip(bestAxis, SplitSurface);
    rightCount = raCount;
    rightBox = raBox;
    rightBox.minClip(bestAxis, SplitSurface);

    double SAH =
        leftBox.surfaceArea() * leftCount + rightBox.surfaceArea() * rightCount;
    if (SAH < bestSAH) {
      bestSAH = SAH;
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

  AABB leftBox = mergeRefs(leftRefs);
  AABB rightBox = mergeRefs(rightRefs);
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
  AABB box = mergeRefs(refs);
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
