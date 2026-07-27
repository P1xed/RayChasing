
#include "Scene.hpp"
#include "Primitives.hpp"
#include "utils.hpp"
#include <cstddef>
#include <fstream>
#include <stdexcept>

namespace rc {
namespace {
using json = nlohmann::json;

void loadSmoothTri(std::vector<SmoothTri> *tris, const json &j) {
  for (auto const &s : j) {
    SmoothTri t;
    rc::staticFor<3>([&t, &s](auto i) {
      t.pos_[i] = {s["positions"][i][0], s["positions"][i][1],
                   s["positions"][i][2]};
    });
    rc::staticFor<3>([&t, &s](auto i) {
      t.nor_[i] = {s["normals"][i][0], s["normals"][i][1], s["normals"][i][2]};
    });
    tris->push_back(t);
  }
}

void loadFlatTri(std::vector<FlatTri> *tris, const json &j) {
  for (auto const &s : j) {
    FlatTri t;
    rc::staticFor<3>([&t, &s](auto i) {
      t.pos_[i] = {s["positions"][i][0], s["positions"][i][1],
                   s["positions"][i][2]};
    });
    t.nor_ = {s["normal"][0], s["normal"][1], s["normal"][2]};
    tris->push_back(t);
  }
}

void loadSph(std::vector<Sph> *sphs, const json &j) {
  for (auto const &s : j) {
    Sph sp;
    sp.cen_ = glm::dvec3{s["center"][0], s["center"][1], s["center"][2]};
    sp.rad_ = s["radius"];
    sphs->push_back(sp);
  }
}

} // namespace

void Scene::loadPrimitives(const json &j) {
  const auto &prims = j["primitives"];
  if (prims.contains("triangles"))
    loadSmoothTri(&smoothTris_, prims["triangles"]);
  if (prims.contains("flat_triangles"))
    loadFlatTri(&flatTris_, prims["flat_triangles"]);
  if (prims.contains("spheres"))
    loadSph(&sphs_, prims["spheres"]);

  for (size_t i = 0; i != smoothTris_.size(); i++)
    primitives.smoothTrisIndexs_.push_back(i);
  for (size_t i = 0; i != flatTris_.size(); i++)
    primitives.flatTrisIndexs_.push_back(i);
  for (size_t i = 0; i != sphs_.size(); i++)
    primitives.sphIndexs_.push_back(i);
}

Scene::Scene(const std::filesystem::path &jsonPath) {
  std::ifstream f(jsonPath);
  if (f.fail() == true)
    throw std::runtime_error("cannot open file: " + jsonPath.string());
  json data = json::parse(f);
  loadPrimitives(data);
}

} // namespace rc
