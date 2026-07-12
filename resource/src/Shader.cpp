
#include "Shader.hpp"
#include "Hit.hpp"
#include <glm/ext/vector_double3.hpp>
#include <glm/geometric.hpp>

namespace rc {

template <>
Pixel Shader<ShaderType::GreyNormal>::shade(const Scene &sc,
                                            const Ray &r) const {
  HitInfo hit;
  for (const auto &obj : sc.flatTris_) {
    obj.intersect(r, &hit);
  }
  for (const auto &obj : sc.smoothTris_) {
    obj.intersect(r, &hit);
  }
  for (const auto &obj : sc.sphs_) {
    obj.intersect(r, &hit);
  }
  if (hit.Miss())
    return {0., 0., 0.};
  double grey = hit.normal_.x;
  // double grey = glm::dot(hit.normal_, glm::normalize(glm::dvec3{1, 1, 1}));
  grey += 1;
  grey *= 0.5;
  return {grey, grey, grey};
}

template <>
Pixel Shader<ShaderType::ColoredNormal>::shade(const Scene &sc,
                                               const Ray &r) const {
  HitInfo hit;
  for (const auto &obj : sc.flatTris_) {
    obj.intersect(r, &hit);
  }
  for (const auto &obj : sc.smoothTris_) {
    obj.intersect(r, &hit);
  }
  for (const auto &obj : sc.sphs_) {
    obj.intersect(r, &hit);
  }
  if (hit.Miss())
    return {0., 0., 0.};
  return {(hit.normal_.x + 1.) * 0.5, (hit.normal_.y + 1.) * 0.5,
          (hit.normal_.z + 1.) * 0.5};
}

} // namespace rc
