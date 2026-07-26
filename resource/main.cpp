
#include "Camera.hpp"
#include "Film.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include <cstddef>
#include <cstdint>
#include <glm/ext/vector_double3.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main() {
  constexpr std::size_t w = 1600;
  constexpr std::size_t h = 900;

  rc::Scene scene("./rc.json");

  rc::Camera camera({0, 0, 0}, 1.6, 0.9, w, h, 1., {0, 1, 0}, {0, 0, -1}, 1);

  rc::Film<w, h> film;
  rc::Shader<rc::ShaderType::ColoredNormal> shader;
  rc::Renderer renderer(scene, camera, shader, film);

  renderer.render();
  std::vector<uint8_t> image;
  renderer.exportImage(&image);

  stbi_write_png("out.png", w, h, 3, image.data(), w * 3);
  std::clog << "Done\n";
  return 0;
}
