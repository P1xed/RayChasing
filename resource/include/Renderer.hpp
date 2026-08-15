
#pragma once

#include "Camera.hpp"
#include "Film.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

namespace rc {

template <ShaderType S, size_t FilmW, size_t FilmH> class Renderer {
  const Scene &scene_;
  const Camera &camera_;
  const Shader<S> &shader_;
  Film<FilmW, FilmH> *film_;

public:
  Renderer(const Scene &scene, const Camera &camera, const Shader<S> &shader,
           Film<FilmW, FilmH> *film)
      : scene_(scene), camera_(camera), shader_(shader), film_(film) {}

  void render() {
    parallelFor<FilmH>([this](size_t y) {
      for (size_t x = 0; x != FilmW; x++) {
        Ray r = camera_.get(x, y);
        HitInfo hit;
        scene_.intersectBVH(r, &hit);
        (*film_)[y * FilmW + x] = shader_.shade(hit);
      }
    });
  }

  void exportImage(std::vector<uint8_t> *image) {
    image->resize(FilmH * FilmW * 3);

    parallelFor<FilmH>([&](size_t y) {
      for (size_t x = 0; x < FilmW; ++x) {
        size_t i = y * FilmW + x;
        Pixel p = (*film_)[i];
#ifndef NDEBUG
        if (p.r_ < 0 or p.r_ > 1)
          std::cerr << "\'R\' out of bound\n";
        if (p.g_ < 0 or p.g_ > 1)
          std::cerr << "\'G\' out of bound\n";
        if (p.b_ < 0 or p.b_ > 1)
          std::cerr << "\'B\' out of bound\n";
#endif
        (*image)[i * 3 + 0] = int(255.99 * p.r_);
        (*image)[i * 3 + 1] = int(255.99 * p.g_);
        (*image)[i * 3 + 2] = int(255.99 * p.b_);
      }
    });
  }
};

} // namespace rc
