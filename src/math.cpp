#include "math.hpp"

namespace nogl
{
  void V4::Normalize(int mask) noexcept
  {
    // Load the vector to begin calculating the inverse magnitude
    XMM<float> vec_128(p_);
    
    XMM<float> inv_mag_128 = vec_128.DotProduct(vec_128, mask).ISquareRoot();

    // Reload it into the register but this time for all its components.
    inv_mag_128 = inv_mag_128.Shuffle(0,0,0,0);
    // Finally the moment we were all waiting for
    vec_128 *= inv_mag_128;
    vec_128.Store(p_);
  }

  void VOV4::Reallocate(unsigned n)
  {
    n_ = n;

    // To fit the 256 alignment.
    unsigned extras = n % (kAlign / sizeof (V4));
    
    buffer_ = std::unique_ptr<V4[]>(
      new (std::align_val_t(kAlign)) V4[n + extras]
    );
  }

  void VOV4::DivideByW(VOV4& output, unsigned from, unsigned to)
  {
    for (unsigned vec = from; vec < to; vec += (kAlign / sizeof(V4)))
    {
      const V4* in_ptr = buffer_.get() + vec;
      V4* out_ptr = output.buffer_.get() + vec;
      
      // Load the 2 vectors into the register
      YMM<float> ab(in_ptr->p_);

      // Load the w components all over the 2 parts of the register
      XMM<float> a = in_ptr[0].p_[3];
      XMM<float> b = in_ptr[1].p_[3];
      YMM<float> w(a,b);

      ab /= w;
      ab.Store(out_ptr->p_);
    }
  }
  
  void VOV4::Multiply(VOV4& output, const M4x4& m, unsigned from, unsigned to) noexcept
  {
    // We do the same thing in V4 but 2 for 1 essentially
    // NOTE: We jump 2 vectors ofc, not 1, but just hu.
    for (unsigned vec = from; vec < to; vec += (kAlign / sizeof(V4)))
    {
      const V4* in_ptr = buffer_.get() + vec;
      V4* out_ptr = output.buffer_.get() + vec;

      YMM<float> res;
      res.ZeroOut();

      for (unsigned i = 0; i < 4; ++i)
      {
        // Load the matrix column into both 128 parts of the AVX
        // __m128 col = _mm_load_ps(m.p_[i]);
        // __m256 cols = reinterpret_cast<__m256>(
        //   _mm256_broadcastsi128_si256(reinterpret_cast<__m128i>(col))
        // );
        YMM<float> cols;
        cols.Broadcast4Floats(m.p_[i]);

        // a is the i-th components COPIED all over from in_ptr[0] and b is same but for in_ptr[1], example [X1,X1,X1,X1 , X0,X0,X0,X0]
        XMM<float> a(in_ptr[0].p_[i]);
        XMM<float> b(in_ptr[1].p_[i]);
        YMM<float> ab(a,b);

        ab *= cols;
        res += ab;
      }

      res.Store(out_ptr->p_);
    }
  }

  void VOV4::Add(VOV4& output, const V4& v, unsigned from, unsigned to)
  {
    for (unsigned vec = from; vec < to; vec += (kAlign / sizeof(V4)))
    {
      const V4* in_ptr = buffer_.get() + vec;
      V4* out_ptr = output.buffer_.get() + vec;

      YMM<float> ab(in_ptr->p_);

      YMM<float> cd;
      cd.Broadcast4Floats(v.p_);

      ab += cd;
      ab.Store(out_ptr->p_);
    }
  }

  void VOV4::Rotate(VOV4& output, const Q4& q, unsigned from, unsigned to)
  {
    YMM<float> q256;
    q256.Broadcast4Floats(q.p_);
    for (unsigned vec = from; vec < to; vec += (kAlign / sizeof(V4)))
    {
      const V4* in_ptr = buffer_.get() + vec;
      V4* out_ptr = output.buffer_.get() + vec;

      YMM<float> ab(in_ptr->p_);

      ab = ab.QVSandwich(q256);
      ab.Store(out_ptr->p_);
    }
  }
}
