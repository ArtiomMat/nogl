#include "math.hpp"

namespace nogl
{
  __m128 _mm_cp_ps(const __m128 left, const __m128 right)
  {
    // Setup the subtracted values
    __m128 sub_left = _mm_mul_ps(
      _mm_shuffle_ps(left,left, _MM_SHUFFLE(3,0,2,1)),
      _mm_shuffle_ps(right,right, _MM_SHUFFLE(3,1,0,2))
    );
    // Setup the subtracting values
    __m128 sub_right = _mm_mul_ps(
      _mm_shuffle_ps(left,left, _MM_SHUFFLE(3,1,0,2)),
      _mm_shuffle_ps(right,right, _MM_SHUFFLE(3,0,2,1))
    );
    // Now do the subtraction and store it back
    return _mm_sub_ps(sub_left, sub_right);
  }

  __m128 _mm_quatmul_ps(const __m128 left, const __m128 right)
  {
    // Scalar part (left[3]*right[3] - dot3(left, right))
    __m128 s_part = _mm_mul_ps(left, right);
    s_part = _mm_sub_ps(
      s_part,
      _mm_dp_ps(left, right, 0b111'1000)
    );

    // Vector part (left[3]*vec3(right) + right[3]*vec3(left) + cross(left, right))
    __m128 tmp, v_part;
    // left[3]*vec3(right)
    v_part = _mm_shuffle_ps(left, left, _MM_SHUFFLE(3,3,3,3));
    v_part = _mm_mul_ps(v_part, right);
    // right[3]*vec3(left)
    tmp = _mm_shuffle_ps(right, right, _MM_SHUFFLE(3,3,3,3));
    tmp = _mm_mul_ps(tmp, left);
    v_part = _mm_add_ps(v_part, tmp);
    // cross(left, right)
    v_part = _mm_add_ps(v_part, _mm_cp_ps(left, right));

    // Blend scalar and vector part
    return _mm_blend_ps(v_part, s_part, 0b1000);
  }

  __m128 _mm_quatvecmul_ps(const __m128 left, const __m128 right)
  {
    // Scalar part (left[3]*0 - dot3(left, right))
    __m128 s_part = _mm_sub_ps(
      _mm_setzero_ps(),
      _mm_dp_ps(left, right, 0b111'1000)
    );

    // Vector part (left[3]*vec3(right) + 0*vec3(left) + cross(left, right))
    __m128 v_part;
    // left[3]*vec3(right)
    v_part = _mm_shuffle_ps(left, left, _MM_SHUFFLE(3,3,3,3));
    v_part = _mm_mul_ps(v_part, right);
    // cross(left, right)
    v_part = _mm_add_ps(v_part, _mm_cp_ps(left, right));

    // Blend scalar and vector part
    return _mm_blend_ps(v_part, s_part, 0b1000);
  }

  __m256 _mm256_cp_ps(const __m256 left, const __m256 right)
  {
    // Setup the subtracted values
    __m256 sub_left = _mm256_mul_ps(
      _mm256_shuffle_ps(left,left, _MM_SHUFFLE(3,0,2,1)),
      _mm256_shuffle_ps(right,right, _MM_SHUFFLE(3,1,0,2))
    );
    // Setup the subtracting values
    __m256 sub_right = _mm256_mul_ps(
      _mm256_shuffle_ps(left,left, _MM_SHUFFLE(3,1,0,2)),
      _mm256_shuffle_ps(right,right, _MM_SHUFFLE(3,0,2,1))
    );
    // Now do the subtraction and store it back
    return _mm256_sub_ps(sub_left, sub_right);
  }

  __m256 _mm256_quatmul_ps(const __m256 left, const __m256 right)
  {
    // Scalar part (left[3]*right[3] - dot3(left, right))
    __m256 s_part = _mm256_mul_ps(left, right);
    s_part = _mm256_sub_ps(
      s_part,
      _mm256_dp_ps(left, right, 0b111'1000)
    );

    // Vector part (left[3]*vec3(right) + right[3]*vec3(left) + cross(left, right))
    __m256 tmp, v_part;
    // left[3]*vec3(right)
    v_part = _mm256_shuffle_ps(left, left, _MM_SHUFFLE(3,3,3,3));
    v_part = _mm256_mul_ps(v_part, right);
    // right[3]*vec3(left)
    tmp = _mm256_shuffle_ps(right, right, _MM_SHUFFLE(3,3,3,3));
    tmp = _mm256_mul_ps(tmp, left);
    v_part = _mm256_add_ps(v_part, tmp);
    // cross(left, right)
    v_part = _mm256_add_ps(v_part, _mm256_cp_ps(left, right));

    // Blend scalar and vector part
    return _mm256_blend_ps(v_part, s_part, 0b1000'1000);
  }

  __m256 _mm256_quatvecmul_ps(const __m256 left, const __m256 right)
  {
    // Scalar part (left[3]*0 - dot3(left, right))
    __m256 s_part = _mm256_sub_ps(
      _mm256_setzero_ps(),
      _mm256_dp_ps(left, right, 0b111'1000)
    );

    // Vector part (left[3]*vec3(right) + 0*vec3(left) + cross(left, right))
    __m256 v_part;
    // left[3]*vec3(right)
    v_part = _mm256_shuffle_ps(left, left, _MM_SHUFFLE(3,3,3,3));
    v_part = _mm256_mul_ps(v_part, right);
    // cross(left, right)
    v_part = _mm256_add_ps(v_part, _mm256_cp_ps(left, right));

    // Blend scalar and vector part
    return _mm256_blend_ps(v_part, s_part, 0b1000'1000);
  }
}
