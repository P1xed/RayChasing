
#pragma once

#include "Camera.hpp"
#include "Film.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>
namespace rc {

template <ShaderType S, std::size_t FilmW, std::size_t FilmH> class Renderer {
  const Scene &scene_;
  const Camera &camera_;
  const Shader<S> &shader_;
  Film<FilmW, FilmH> &film_;

public:
  Renderer(const Scene &scene, const Camera &camera, const Shader<S> &shader,
           Film<FilmW, FilmH> &film)
      : scene_(scene), camera_(camera), shader_(shader), film_(film) {}

  void render() {
    for (std::size_t y = 0; y != FilmH; y++)
      for (std::size_t x = 0; x != FilmW; x++)
        film_[y * FilmW + x] = shader_.shade(scene_, camera_.get(x, y));
  }

  void exportImage(std::vector<uint8_t> *image) {
    image->resize(FilmH * FilmW * 3);
    for (std::size_t i = 0; i != FilmH * FilmW; i++) {
      auto p = film_[i];
#ifndef NDEBUG
      if (p.r < 0 or p.r > 1)
        std::clog << "\'R\' out of bound\n";
      if (p.g < 0 or p.g > 1)
        std::clog << "\'G\' out of bound\n";
      if (p.b < 0 or p.b > 1)
        std::clog << "\'B\' out of bound\n";
#endif
      (*image)[i * 3 + 0] = int(255.99 * p.r);
      (*image)[i * 3 + 1] = int(255.99 * p.g);
      (*image)[i * 3 + 2] = int(255.99 * p.b);
    }
  }
};

} // namespace rc
