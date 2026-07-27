
#include "Shader.hpp"
#include "Hit.hpp"
#include "Scene.hpp"
#include <glm/ext/vector_double3.hpp>
#include <glm/geometric.hpp>

namespace rc {

template <>
Pixel Shader<ShaderType::GreyNormal>::shade(const Scene &sc,
                                            const Ray &r) const {
  HitInfo hit;
  sc.primitives.intersect(r, &hit);
  if (hit.Miss())
    return {0., 0., 0.};
  double grey = 0.5 * (hit.normal_.x + 1);
  return {grey, grey, grey};
}

template <>
Pixel Shader<ShaderType::ColoredNormal>::shade(const Scene &sc,
                                               const Ray &r) const {
  HitInfo hit;
  sc.primitives.intersect(r, &hit);
  if (hit.Miss())
    return {0., 0., 0.};
  return {(hit.normal_.x + 1.) * 0.5, (hit.normal_.y + 1.) * 0.5,
          (hit.normal_.z + 1.) * 0.5};
}

} // namespace rc
