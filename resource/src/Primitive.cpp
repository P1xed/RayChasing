
#include "Primitive.hpp"

#include <algorithm>
#include <cmath>
#include <glm/geometric.hpp>
#include <utility>

void rc::SmoothTri::intersect(const Ray &r, HitInfo *h) const {
  glm::dvec3 e1 = pos_[1] - pos_[0];
  glm::dvec3 e2 = pos_[2] - pos_[0];
  glm::dvec3 p0O = r.orig_ - pos_[0];
  double detM = glm::dot(r.dir_, glm::cross(e1, e2));
  if (std::abs(detM) <= 0.)
    return;
  double invDet = 1. / detM;
  double t = -glm::dot(p0O, glm::cross(e1, e2)) * invDet;
  double u = glm::dot(r.dir_, glm::cross(p0O, e2)) * invDet;
  double v = glm::dot(r.dir_, glm::cross(e1, p0O)) * invDet;
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
  double maxX = std::max({pos_[0].x, pos_[1].x, pos_[2].x});
  double maxY = std::max({pos_[0].y, pos_[1].y, pos_[2].y});
  double maxZ = std::max({pos_[0].z, pos_[1].z, pos_[2].z});
  double minX = std::min({pos_[0].x, pos_[1].x, pos_[2].x});
  double minY = std::min({pos_[0].y, pos_[1].y, pos_[2].y});
  double minZ = std::min({pos_[0].z, pos_[1].z, pos_[2].z});
  return {{minX, minY, minZ}, {maxX, maxY, maxZ}};
}

glm::dvec3 rc::SmoothTri::getCentroid() const {
  constexpr double inv3 = 1. / 3.;
  return (pos_[0] + pos_[1] + pos_[2]) * inv3;
}

void rc::FlatTri::intersect(const Ray &r, HitInfo *h) const {
  glm::dvec3 e1 = pos_[1] - pos_[0];
  glm::dvec3 e2 = pos_[2] - pos_[0];
  glm::dvec3 p0O = r.orig_ - pos_[0];
  double detM = glm::dot(r.dir_, glm::cross(e1, e2));
  if (std::abs(detM) <= 0.)
    return;
  double invDet = 1. / detM;
  double t = -glm::dot(p0O, glm::cross(e1, e2)) * invDet;
  double u = glm::dot(r.dir_, glm::cross(p0O, e2)) * invDet;
  double v = glm::dot(r.dir_, glm::cross(e1, p0O)) * invDet;
  if (u < 0. or v < 0. or u + v > 1.)
    return;
  if (t <= h->tMin_ || t >= h->tMax_)
    return;
  h->tMax_ = t;
  h->point_ = r.at(t);
  h->normal_ = glm::normalize(nor_);
}

rc::AABB rc::FlatTri::getAABB() const {
  double maxX = std::max({pos_[0].x, pos_[1].x, pos_[2].x});
  double maxY = std::max({pos_[0].y, pos_[1].y, pos_[2].y});
  double maxZ = std::max({pos_[0].z, pos_[1].z, pos_[2].z});
  double minX = std::min({pos_[0].x, pos_[1].x, pos_[2].x});
  double minY = std::min({pos_[0].y, pos_[1].y, pos_[2].y});
  double minZ = std::min({pos_[0].z, pos_[1].z, pos_[2].z});
  return {{minX, minY, minZ}, {maxX, maxY, maxZ}};
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
  double t0 = (-halfB - sqrt(discriminant)) / a;
  double t1 = (-halfB + sqrt(discriminant)) / a;
  if (t0 < h->tMin_ or t0 > h->tMax_){
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
