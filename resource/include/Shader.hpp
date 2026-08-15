
#pragma once
#include "Hit.hpp"
#include "Pixel.hpp"

namespace rc {

enum class ShaderType { GreyNormal, ColoredNormal, OWRC, PBR };

template <ShaderType S> class Shader {
public:
  Pixel shade(const HitInfo &hit) const;
};

} // namespace rc
