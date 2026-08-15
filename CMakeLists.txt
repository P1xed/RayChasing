cmake_minimum_required(VERSION 4.4.0)

project(RayChasing CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
  rc
  ./resource/main.cpp
  ./resource/src/Scene.cpp
  ./resource/src/Shader.cpp
  ./resource/src/PrimitiveView.cpp
  ./resource/src/Primitive.cpp
  ./resource/src/BVH.cpp
  ./resource/include/Buffer.hpp
  ./resource/include/Camera.hpp
  ./resource/include/Film.hpp
  ./resource/include/Pixel.hpp
  ./resource/include/Primitive.hpp
  ./resource/include/Ray.hpp
  ./resource/include/Renderer.hpp
  ./resource/include/Scene.hpp
  ./resource/include/Shader.hpp
  ./resource/include/utils.hpp
  ./resource/include/ThreadPool.hpp
  ./resource/include/PrimitiveView.hpp
)

target_include_directories(rc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/include
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/lib
)
