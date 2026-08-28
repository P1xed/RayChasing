
#include "Scene.hpp"
#include "PrimitiveView.hpp"
#include "utils.hpp"
#include <cstddef>
#include <fstream>
#include <stdexcept>
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

  primitives_.primitiveIndexs_.reserve(smoothTris_.size()+flatTris_.size()+sphs_.size());
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
  std::vector<BVHRef> refs;
  refs.reserve(primitives_.primitiveIndexs_.size());
  for (const taggedIdx &p : primitives_.primitiveIndexs_) {
    AABB a = visitPrimitive(*this, p, [](const auto &prim) { return prim.getAABB(); });
    refs.push_back({p, a});
  }
  BVHNode::build(std::move(refs), this);
}

void Scene::intersectBVH(const Ray &r, HitInfo *h) const {

  struct Item {
    size_t idx_;
    double tNear_;
  };

  double t;
  if (!BVHNodes_[0].box_.intersect(r, h->tMin_, h->tMax_, &t))
    return; // TODO:mini-tree maybe better performance 

  std::vector<Item> stack;
  stack.reserve(128);
  stack.push_back({0, t});

  for (; !stack.empty();) {
    Item it = stack.back();
    stack.pop_back();
    if (it.tNear_ >= h->tMax_)
      continue;

    const BVHNode &n = BVHNodes_[it.idx_];
    if (n.left_ == BVH_LEAF_FLAG) {
      BVHLeaves_[n.right_].primitives_.intersect(*this, r, h);
      continue;
    }

    double leftT, rightT;
    bool ifHitLeft =
        BVHNodes_[n.left_].box_.intersect(r, h->tMin_, h->tMax_, &leftT);
    bool ifHitRight =
        BVHNodes_[n.right_].box_.intersect(r, h->tMin_, h->tMax_, &rightT);
    if (ifHitLeft && ifHitRight) {
      if (leftT <= rightT) {
        stack.push_back({n.right_, rightT});
        stack.push_back({n.left_, leftT});
      } else {
        stack.push_back({n.left_, leftT});
        stack.push_back({n.right_, rightT});
      }
    } else if (ifHitLeft)
      stack.push_back({n.left_, leftT});
    else if (ifHitRight)
      stack.push_back({n.right_, rightT});
  }
} // TODO: bin split for better BVH tree quality

} // namespace rc
