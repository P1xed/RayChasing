
#pragma once
#include "Primitive.hpp"
#include "Primitives.hpp"
#include "json.hpp"
#include <filesystem>
#include <vector>

namespace rc {

class Scene {
public:
  std::vector<SmoothTri> smoothTris_;
  std::vector<FlatTri> flatTris_;
  std::vector<Sph> sphs_;
  Primitives primitives = Primitives(&smoothTris_, &flatTris_, &sphs_);

  Scene(const Scene &) = delete;
  Scene &operator=(const Scene &) = delete;
  Scene(Scene &&) = delete;
  Scene &operator=(Scene &&) = delete;
  explicit Scene(const std::filesystem::path &jsonPath);

private:
  void loadPrimitives(const nlohmann::json &j);
};

} // namespace rc
