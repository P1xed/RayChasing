
#pragma once
#include "Pixel.hpp"
#include "Ray.hpp"
#include "Scene.hpp"

namespace rc {

enum class ShaderType { GreyNormal, ColoredNormal, OWRC, PBR };

template <ShaderType S> class Shader {
public:
  Pixel shade(const Scene &sc, const Ray &r) const;
};

} // namespace rc
