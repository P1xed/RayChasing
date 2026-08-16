
#include "Primitive.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <glm/geometric.hpp>
#include <utility>

void rc::SmoothTri::intersect(const Ray &r, HitInfo *h) const {
  glm::dvec3 e1 = pos_[1] - pos_[0];
  glm::dvec3 e2 = pos_[2] - pos_[0];
  glm::dvec3 pvec = glm::cross(r.dir_, e2);
  double det = glm::dot(e1, pvec);
  if (std::abs(det) <= 0.)
    return;
  double invDet = 1. / det;
  glm::dvec3 tvec = r.orig_ - pos_[0];
  double u = glm::dot(tvec, pvec) * invDet;
  glm::dvec3 qvec = glm::cross(tvec, e1);
  double v = glm::dot(r.dir_, qvec) * invDet;
  double t = glm::dot(e2, qvec) * invDet;
  if (u < 0. or v < 0. or u + v > 1.)
    return;
  if (t <= h->tMin_ || t >= h->tMax_)
    return;
  h->tMax_ = t;
  h->point_ = r.at(t);
  h->normal_ =
      glm::normalize((1. - u - v) * nor_[0] + u * nor_[1] + v * nor_[2]);
}

rc::AABB rc::SmoothTri::getAABB() const {
  glm::dvec3 min, max;
  staticFor<3>([&](size_t axis) {
    min[axis] = std::min({pos_[0][axis], pos_[1][axis], pos_[2][axis]});
    max[axis] = std::max({pos_[0][axis], pos_[1][axis], pos_[2][axis]});
  });
  return {min, max};
}

glm::dvec3 rc::SmoothTri::getCentroid() const {
  constexpr double inv3 = 1. / 3.;
  return (pos_[0] + pos_[1] + pos_[2]) * inv3;
}

void rc::FlatTri::intersect(const Ray &r, HitInfo *h) const {
  glm::dvec3 e1 = pos_[1] - pos_[0];
  glm::dvec3 e2 = pos_[2] - pos_[0];
  glm::dvec3 pvec = glm::cross(r.dir_, e2);
  double det = glm::dot(e1, pvec);
  if (std::abs(det) <= 0.)
    return;
  double invDet = 1. / det;
  glm::dvec3 tvec = r.orig_ - pos_[0];
  double u = glm::dot(tvec, pvec) * invDet;
  glm::dvec3 qvec = glm::cross(tvec, e1);
  double v = glm::dot(r.dir_, qvec) * invDet;
  double t = glm::dot(e2, qvec) * invDet;
  if (u < 0. or v < 0. or u + v > 1.)
    return;
  if (t <= h->tMin_ || t >= h->tMax_)
    return;
  h->tMax_ = t;
  h->point_ = r.at(t);
  h->normal_ = glm::normalize(nor_);
}

rc::AABB rc::FlatTri::getAABB() const {
  glm::dvec3 min, max;
  staticFor<3>([&](size_t axis) {
    min[axis] = std::min({pos_[0][axis], pos_[1][axis], pos_[2][axis]});
    max[axis] = std::max({pos_[0][axis], pos_[1][axis], pos_[2][axis]});
  });
  return {min, max};
}

glm::dvec3 rc::FlatTri::getCentroid() const {
  constexpr double inv3 = 1. / 3.;
  return (pos_[0] + pos_[1] + pos_[2]) * inv3;
}

void rc::Sph::intersect(const Ray &r, HitInfo *h) const {
  glm::dvec3 CO = r.orig_ - cen_;
  double a = glm::dot(r.dir_, r.dir_);
  double halfB = glm::dot(r.dir_, CO);
  double c = glm::dot(CO, CO) - rad_ * rad_;
  double discriminant = halfB * halfB - a * c;
  if (discriminant < 0.)
    return;
  double q = -halfB - std::copysign(std::sqrt(discriminant), halfB);
  double t0 = q / a;
  double t1 = c / q;
  if (t0 > t1)
    std::swap(t0, t1);
  if (t0 < h->tMin_ or t0 > h->tMax_) {
    if (t1 < h->tMin_ or t1 > h->tMax_)
      return;
    std::swap(t0, t1);
  }
  glm::dvec3 point = r.at(t0);
  h->tMax_ = t0;
  h->point_ = point;
  h->normal_ = (point - cen_) / rad_;
}

rc::AABB rc::Sph::getAABB() const { return {cen_ - rad_, cen_ + rad_}; }

glm::dvec3 rc::Sph::getCentroid() const { return cen_; }
