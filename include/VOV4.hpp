#pragma once

#include "math.hpp"
#include "V4.hpp"
#include "M4x4.hpp"
#include "Q4.hpp"
#include "math.hpp"
#include "Exception.hpp"

namespace nogl
{
  // Vector Of Vectors(4 dimensional)
  class VOV4
  {
    public:

    static constexpr unsigned kAlign = sizeof(__m256); // Using the 256-bit AVX/SSE SIMD

    // n is the number of the vectors.
    VOV4(unsigned n = 0)
    {
      Reallocate(n);
    }
    VOV4(const VOV4& other) : VOV4(other.n_)
    {
      *this = other;
    }

    ~VOV4() = default;

    // NOTE: Erases all previous data if existed.
    void Reallocate(unsigned n);
    // Assuming v is the size of `n()`, copies float triplets into the VOV with the W component being `0`.
    // NOTE: Since V3 is only aligned to 8 bytes and is 12 bytes, it makes for a challenging copy.
    // void operator =(V3* v)
    // {

    // }

    // Set every single vector, and every one of its components to `f`.
    void operator =(float f) noexcept
    {
      __m256 filler = _mm256_set1_ps(f);
      for (V4* ptr = begin(); ptr < end(); ptr += (kAlign / sizeof(V4)))
      {
        _mm256_store_ps(ptr->p_, filler);
      }
    }
    // Assume f is the size of `4*n()`, so it contains all the vectors necessary flattened into an array.
    void operator =(float* f) noexcept
    {
      for (V4* ptr = begin(); ptr < end(); ptr += (kAlign / sizeof(V4)))
      {
        _mm256_store_ps(ptr->p_, _mm256_load_ps(f));
      }
    }
    void operator =(const V4& v) noexcept
    {
      __m256 v_256 = _mm256_broadcast_ps(reinterpret_cast<const __m128*>(v.p_));
      for (V4* ptr = begin(); ptr < end(); ptr += (kAlign / sizeof(V4)))
      {
        _mm256_store_ps(ptr->p_, v_256);
      }
    }
    // Copies vectors from `other` to `this`, the number of vectors is whoever has a smaller `n`.
    void operator =(const VOV4& other) noexcept
    {
      for (unsigned off = 0; off < std::min(n_, other.n_); off += (kAlign / sizeof (V4)))
      {
        _mm256_store_ps(
          buffer_[off].p_,
          _mm256_load_ps(other.buffer_[off].p_)
        );
      }
    }

    void operator *=(const M4x4& m) noexcept { Multiply(*this, m, 0, n_); }
    
    // Multiplies all vectors by `matrix`(as if our vectors are 1x4 matrices), stores results in `output`(can be `*this`).
    // From `from` up to `to`(exclusive).
    // Huge note: The address in bytes of `from` & `to` must be aligned to `kAlign`.
    void Multiply(VOV4& output, const M4x4& m, unsigned from, unsigned to) noexcept;
    
    // Divides each vector by its own W component, stores results in `output`(can be `*this`).
    void DivideByW(VOV4& output, unsigned from, unsigned to);

    // Subtracts all vectors with `v`, stores results in `output`(can be `*this`).
    // From `from` up to `to`(exclusive).
    // Huge note: The address in bytes of `from` & `to` must be aligned to `kAlign`.
    void Subtract(VOV4& output, const V4& v, unsigned from, unsigned to);
    // Same as `Add()` for V4, refers to `v` as if it's `V4::p()`.
    // void Add(VOV4& output, const float v[4], unsigned from, unsigned to);

    // Rotate each vector using the quaternion `q`.
    void Rotate(VOV4& output, const Q4& q, unsigned from, unsigned to);

    // A chunk is a piece that a single Minion may process at once.
    // unsigned chunk_size(unsigned total_n) { return (n_ / (kAlign / sizeof (V4))) / total_n; }

    // The number of vectors, not bytes, not floats, remember that before you refer to it as something else.
    unsigned n() const noexcept { return n_; }
    const V4* begin() const noexcept { return buffer_.get(); }
    const V4* end() const noexcept { return buffer_.get() + n_; }
    V4* begin() noexcept { return buffer_.get(); }
    V4* end() noexcept { return buffer_.get() + n_; }

    // Returns the vector at index `i`, avoid copying it, because then it will not be that vector.
    // NOTE: The object returned directly references the actual vector from this VOV, so any operation you do on the  object affects that vector here. 
    V4& operator [](unsigned i) noexcept { return buffer_[i]; }
    const V4& operator [](unsigned i) const noexcept { return buffer_[i]; }

    private:
    // See `n()`
    unsigned n_;
    // MUST BE ALIGNED TO `kAlign`
    std::unique_ptr<V4[]> buffer_ = nullptr;
  };
}
