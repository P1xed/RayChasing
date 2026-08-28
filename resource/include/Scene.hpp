
#pragma once
#include "Primitive.hpp"
#include "PrimitiveView.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <vector>

namespace rc {

class Scene {
public:
  std::vector<SmoothTri> smoothTris_;
  std::vector<FlatTri> flatTris_;
  std::vector<Sph> sphs_;
  std::vector<BVHNode> BVHNodes_;
  std::vector<BVHLeaf> BVHLeaves_;
  PrimitiveView primitives_;

  Scene(const Scene &) = delete;
  Scene &operator=(const Scene &) = delete;
  Scene(Scene &&) = delete;
  Scene &operator=(Scene &&) = delete;
  explicit Scene(const std::filesystem::path &jsonPath);
  void buildBVH();
  void intersectBVH(const Ray &r, HitInfo *h) const;

private:
  void loadPrimitives(const nlohmann::json &j);
};

} // namespace rc
