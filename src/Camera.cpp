#include "Camera.hpp"

#include <cmath>

namespace nogl
{
  void Camera::RecalculateMatrix()
  {
    // float a = context_->width() / context_->height();
    // The matrix is slightly different from your classic perspective projection matrix, in that the width and height are baked into it in such a way that what we actually get is a range of -width/2 to width/2 and same for height, then the job of the rasterizer is to also add to each vertex [width/2, height/2, 0, 0], this speeds things up a little.
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
  void Camera::set_aspect_ratio(float a)
  {
    aspect_ratio_ = a;
    RecalculateMatrix();
  }
}