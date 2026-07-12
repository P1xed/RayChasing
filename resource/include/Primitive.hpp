
#pragma once
#include "Hit.hpp"
#include "Ray.hpp"
#include <array>
#include <cmath>
#include <glm/ext/vector_double3.hpp>
#include <glm/geometric.hpp>

namespace rc {

struct SmoothTri {
  std::array<glm::dvec3, 3> pos_;
  std::array<glm::dvec3, 3> nor_;
  void intersect(const Ray &r, HitInfo *h) const {
    // (O-P0)=M⋅[t,u,v]^T, M=[-D,e1,e2]
    auto e1 = pos_[1] - pos_[0];
    auto e2 = pos_[2] - pos_[0];
    auto p0O = r.orig_ - pos_[0];
    auto detM = glm::dot(r.dir_, glm::cross(e1, e2));
    if (std::abs(detM) <= 0.)
      return;
    auto invDet = 1. / detM;
    auto t = -glm::dot(p0O, glm::cross(e1, e2)) * invDet;
    auto u = glm::dot(r.dir_, glm::cross(p0O, e2)) * invDet;
    auto v = glm::dot(r.dir_, glm::cross(e1, p0O)) * invDet;
    if (u < 0. or v < 0. or u + v > 1.)
      return;
    if (t <= h->tMin_ || t >= h->tMax_)
      return;
    h->tMax_ = t;
    h->point_ = r.at(t);
    h->normal_ =
        glm::normalize((1. - u - v) * nor_[0] + u * nor_[1] + v * nor_[2]);
  }
};

struct FlatTri {
  std::array<glm::dvec3, 3> pos_;
  glm::dvec3 nor_;
  void intersect(const Ray &r, HitInfo *h) const {
    // (O-P0)=M⋅[t,u,v]^T, M=[-D,e1,e2]
    auto e1 = pos_[1] - pos_[0];
    auto e2 = pos_[2] - pos_[0];
    auto p0O = r.orig_ - pos_[0];
    auto detM = glm::dot(r.dir_, glm::cross(e1, e2));
    if (std::abs(detM) <= 0.)
      return;
    auto invDet = 1. / detM;
    auto t = -glm::dot(p0O, glm::cross(e1, e2)) * invDet;
    auto u = glm::dot(r.dir_, glm::cross(p0O, e2)) * invDet;
    auto v = glm::dot(r.dir_, glm::cross(e1, p0O)) * invDet;
    if (u < 0. or v < 0. or u + v > 1.)
      return;
    if (t <= h->tMin_ || t >= h->tMax_)
      return;
    h->tMax_ = t;
    h->point_ = r.at(t);
    h->normal_ = glm::normalize(nor_);
  }
};

struct Sph {
  glm::dvec3 cen_;
  double rad_;
  void intersect(const Ray &r, HitInfo *h) const {
    // |O+tD-C|²= r²
    auto CO = r.orig_ - cen_;
    auto a = glm::dot(r.dir_, r.dir_);
    // auto b = 2. * glm::dot(r.dir_, CO);
    auto halfB = glm::dot(r.dir_, CO);
    auto c = glm::dot(CO, CO) - rad_ * rad_;

    auto discriminant = halfB * halfB - a * c;
    if (discriminant < 0.)
      return;

    auto t0 = (-halfB - sqrt(discriminant)) / a;
    auto t1 = (-halfB + sqrt(discriminant)) / a;
    double t;
    if (t0 > h->tMin_ and t0 < h->tMax_)
      t = t0;
    else if (t1 > h->tMin_ and t1 < h->tMax_)
      t = t1;
    else
      return;

    auto point = r.at(t);
    h->tMax_ = t;
    h->point_ = point;
    h->normal_ = (point - cen_) / rad_;

    return;
  }
};
} // namespace rc

// TODO:
// multi primitive insert requires tagged pointer
