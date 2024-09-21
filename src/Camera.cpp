#include "Camera.hpp"

#include <cmath>

namespace nogl
{
  void Camera::RecalculateMatrix()
  {
    float m[] = {
      1 / (tanf(yfov_ / 2) * aspect_ratio_), 0, 0, 0,
      0, 1 / tanf(yfov_ / 2), 0, 0,
      0, 0, - (znear_ + zfar_) / (zfar_ - znear_), - (2 * zfar_ * znear_) / (zfar_ - znear_),
      0, 0, -1, 0,
    };
    matrix_ = m;
  }

  void Camera::set_zfar(float zfar)
  {
    zfar_ = zfar;
    RecalculateMatrix();
  }
  void Camera::set_znear(float znear)
  {
    znear_ = znear;
    RecalculateMatrix();
  }

  void Camera::set_yfov(float yfov)
  {
    yfov_ = yfov;
    RecalculateMatrix();
  }
  void Camera::set_aspect_ratio(float aspect_ratio)
  {
    aspect_ratio_ = aspect_ratio;
    RecalculateMatrix();
  }
}