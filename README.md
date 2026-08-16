# RayChasing

A simple soft renderer library for multicore CPU rendering in C++
>for personal study

## Dependencies

- pcg
- glm
- nlohmann json

## Build

```bash
cmake -S . -B build && cmake --build build/ && ./build/rc 
```

## Todos

- [ ] write a renderer
  - [x] write a buffer header
  - [x] write a scene parser
  - [x] write a camera
  - [ ] implement a thread pool(memory pool later)
  - [x] implement tagged index for multi-primitive intersect(to optimize maybe?)
  - [ ] use .bin file for better storing
  - [ ] BVH optimization
    - [x] SBVH
    - [ ] bin split
    - [ ] mini-tree
    - [ ] export BVH build method to main.cpp(can select performance later)
- [ ] make sure cross platform

## Final Goal

To render an image purely by CPU that looks **beautiful** at the same time, **fast**

## Reference

- [Physically Based Rendering](https://pbr-book.org/4ed/contents)
- [Ray Tracing in One Weekend](https://raytracing.github.io/)

## License

Distributed under the MIT License. See [LICENSE](./LICENSE) for more information.
