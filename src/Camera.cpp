#include "Camera.hpp"

#include <cmath>

namespace nogl
{
  void Camera::RecalculateMatrix()
  {
    float a = width_ / height_;
    // The matrix is slightly different from your classic perspective projection matrix, in that the width and height are baked into it in such a way that what we actually get is a range of 0 to width, and same for height.
    // For width: This is done by first slapping width_/2 on the dividend, and then on the z's column we put -width_/2, this way we add z*-width_/2, so when we divide by w which is just -z, we get width/2 added to the x axis, which essentially shifts the x component range -width_/2..width_/2 to 0..width_.
    float m[] = {
      (width_/2) / (tanf(yfov_ / 2) * a), 0, -width_/2, 0,
      0, (height_/2) / tanf(yfov_ / 2), -height_/2, 0,
      0, 0, - (zfar_ + znear_) / (zfar_ - znear_), - (2 * zfar_ * znear_) / (zfar_ - znear_),
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
}