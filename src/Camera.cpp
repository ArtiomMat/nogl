#include "Camera.hpp"

#include <cmath>

namespace nogl
{
  void Camera::RecalculateMatrix()
  {
    float it = 1.0f / tanf(yfov_ / 2.0f);
    float znmzf = znear_ - zfar_;

    float m[] = {
      it / aspect_ratio_, 0, 0, 0,
      0, it, 0, 0,
      0, 0, - (znear_ + zfar_) / znmzf, (2 * znear_ * zfar_) / znmzf,
      0, 0, 1, 0,
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