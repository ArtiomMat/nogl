#include "Camera.hpp"

#include <cmath>

namespace nogl
{
  void Camera::RecalculateMatrix()
  {
    // TODO: Literally bake the screen width/2 and height/2 into the matrix, you multiply it all and shift it all anyway, so why not do that technique?
    // Though one thing we can't do is add width/2 and height/2 to the result(due to division by W which will screw things up), but one solution is to do it via Add() but with a vector which equals [width/2, height/2, 0, 0].
    // Another thing is that the camera directly depends on the context, so gotta perhaps make the context a singleton.
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