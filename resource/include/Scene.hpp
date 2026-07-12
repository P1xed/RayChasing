
#pragma once
#include "Primitive.hpp"
#include "json.hpp"
#include <filesystem>
#include <vector>

namespace rc {

class Scene {
  // TODO:
  // implement muiltple primitive support
public:
  std::vector<SmoothTri> smoothTris_;
  std::vector<FlatTri> flatTris_;
  std::vector<Sph> sphs_;
  // TODO: they should not be accessable

  explicit Scene(const std::filesystem::path &jsonPath);

private:
  void loadPrimitives(const nlohmann::json &j);
};

} // namespace rc
