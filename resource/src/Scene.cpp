
#include "Scene.hpp"
#include "PrimitiveView.hpp"
#include "utils.hpp"
#include <array>
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rc {
namespace {
using json = nlohmann::json;

void loadSmoothTri(std::vector<SmoothTri> *tris, const json &j) {
  (*tris).reserve(j.size());
  for (json const &s : j) {
    SmoothTri t;
    rc::static_for<3>([&t, &s](auto i) {
      t.pos_[i] = {s["positions"][i][0], s["positions"][i][1],
                   s["positions"][i][2]};
    });
    rc::static_for<3>([&t, &s](auto i) {
      t.nor_[i] = {s["normals"][i][0], s["normals"][i][1], s["normals"][i][2]};
    });
    tris->push_back(t);
  }
}

void loadFlatTri(std::vector<FlatTri> *tris, const json &j) {
  (*tris).reserve(j.size());
  for (json const &s : j) {
    FlatTri t;
    rc::static_for<3>([&t, &s](auto i) {
      t.pos_[i] = {s["positions"][i][0], s["positions"][i][1],
                   s["positions"][i][2]};
    });
    t.nor_ = {s["normal"][0], s["normal"][1], s["normal"][2]};
    tris->push_back(t);
  }
}

void loadSph(std::vector<Sph> *sphs, const json &j) {
  (*sphs).reserve(j.size());
  for (json const &s : j) {
    Sph sp;
    sp.cen_ = glm::dvec3{s["center"][0], s["center"][1], s["center"][2]};
    sp.rad_ = s["radius"];
    sphs->push_back(sp);
  }
}

} // namespace

void Scene::loadPrimitives(const json &j) {
  const json &prims = j["primitives"];
  if (prims.contains("triangles"))
    loadSmoothTri(&smoothTris_, prims["triangles"]);
  if (prims.contains("flat_triangles"))
    loadFlatTri(&flatTris_, prims["flat_triangles"]);
  if (prims.contains("spheres"))
    loadSph(&sphs_, prims["spheres"]);

  primitives_.primitiveIndexs_.reserve(smoothTris_.size() + flatTris_.size() +
                                       sphs_.size());
  for (size_t i = 0; i != smoothTris_.size(); i++)
    primitives_.primitiveIndexs_.push_back(
        taggedIdx(static_cast<uint64_t>(i), PrimitiveType::SmoothTri));
  for (size_t i = 0; i != flatTris_.size(); i++)
    primitives_.primitiveIndexs_.push_back(
        taggedIdx(static_cast<uint64_t>(i), PrimitiveType::FlatTri));
  for (size_t i = 0; i != sphs_.size(); i++)
    primitives_.primitiveIndexs_.push_back(
        taggedIdx(static_cast<uint64_t>(i), PrimitiveType::Sph));
}

Scene::Scene(const std::filesystem::path &jsonPath) {
  std::ifstream f(jsonPath);
  if (f.fail() == true)
    throw std::runtime_error("cannot open file: " + jsonPath.string());
  json data = json::parse(f);
  loadPrimitives(data);
  buildBVH();
}

void Scene::buildBVH() {
  BVHNodes_.clear();
  BVHLeaves_.clear();
  BVHNode::buildRoot(this);
}

void Scene::intersectBVH(const Ray &r, HitInfo *h) const {

  double t;
  if (!BVHNodes_[0].box_.intersect(r, h->tMin_, h->tMax_, &t))
    return;

  std::array<std::pair<size_t, double>, 128> BVHNodeStack;
  size_t sp = 0;
  BVHNodeStack[sp++] = {0, t};

  while (sp > 0) {
    auto &[idx, tNear] = BVHNodeStack[--sp];
    if (tNear >= h->tMax_)
      continue;

    const BVHNode &n = BVHNodes_[idx];
    if (n.left_ == BVH_LEAF_FLAG) {
      BVHLeaves_[n.right_].primitives_.intersect(*this, r, h);
      continue;
    } else {

      double leftT, rightT;
      bool ifHitLeft =
          BVHNodes_[n.left_].box_.intersect(r, h->tMin_, h->tMax_, &leftT);
      bool ifHitRight =
          BVHNodes_[n.right_].box_.intersect(r, h->tMin_, h->tMax_, &rightT);
      if (ifHitLeft && ifHitRight) {
        if (leftT <= rightT) {
          BVHNodeStack[sp++] = {n.right_, rightT};
          BVHNodeStack[sp++] = {n.left_, leftT};
        } else {
          BVHNodeStack[sp++] = {n.left_, leftT};
          BVHNodeStack[sp++] = {n.right_, rightT};
        }
      } else if (ifHitLeft)
        BVHNodeStack[sp++] = {n.left_, leftT};
      else if (ifHitRight)
        BVHNodeStack[sp++] = {n.right_, rightT};
    }
  }
}

} // namespace rc
