#pragma once

namespace nogl
{
  // All opeartor overloads prefer Order::kSeqCst or next most strict Order types.
  template <typename T>
  class Atomic
  {
    public:
    enum class Order : int
    {
      kRelaxed = __ATOMIC_RELAXED,
      kConsume = __ATOMIC_CONSUME,
      kAcquire = __ATOMIC_ACQUIRE,
      kRelease = __ATOMIC_RELEASE,
      kAcqRel = __ATOMIC_ACQ_REL,
      kSeqCst = __ATOMIC_SEQ_CST,
    };

    Atomic() = default;
    ~Atomic() = default;

    T Load(Order o) { return __atomic_load_n(&data, o); }
    operator T() { return Load(Order::kSeqCst); }

    void Store(T v, Order o) { __atomic_store(&data, v, o); }
    // void operator =(T v) { Store(v, Order::kSeqCst); }
    
    T Exchange(T v, Order o) { return __atomic_exchange_n(&data, v, o); }
    // `Exchange()`, if you want just to store use `Store()`, slightly faster if you don't need to fetch.
    T operator =(T v) { return Exchange(v, Order::kSeqCst); }
    
    T FetchAdd(T v, Order o) { return __atomic_fetch_add(&data, v, o); }
    // `FetchAdd()`
    T operator +=(T v) { return FetchAdd(v, Order::kSeqCst); }
    // `+=1`
    T operator ++(T v) { return *this += 1; }
    
    T FetchSub(T v, Order o) { return __atomic_fetch_sub(&data, v, o); }
    // `FetchSub()`
    T operator -=(T v) { return FetchSub(v, Order::kSeqCst); }
    // `-=1`
    T operator --(T v) { return *this -= 1; }
    
    T FetchXor(T v, Order o) { return __atomic_fetch_xor(&data, v, o); }
    // `FetchXor()`
    T operator ^=(T v) { return FetchXor(v, Order::kSeqCst); }
    
    T FetchOr(T v, Order o) { return __atomic_fetch_xor(&data, v, o); }
    // `FetchOr()`
    T operator |=(T v) { return FetchOr(v, Order::kSeqCst); }
    
    T FetchAnd(T v, Order o) { return __atomic_fetch_xor(&data, v, o); }
    // `FetchAnd()`
    T operator &=(T v) { return FetchAnd(v, Order::kSeqCst); }
    
    // Aparently no NAND operator in C and C++???
    T FetchNand(T v, Order o) { return __atomic_fetch_xor(&data, v, o); }


    bool CompareExchange(T* expected, T desired, bool weak, Order success_order, Order fail_order)
    {
      return __atomic_compare_exchange_n(&data, expected, desired, weak, success_order, fail_order);
    }
    bool CompareExchange(T expected, T desired, bool weak, Order success_order, Order fail_order)
    {
      return CompareExchange(&data, &expected, desired, weak, success_order, fail_order);
    }
    
    bool CompareExchange(T* expected, T desired, bool weak)
    {
      return CompareExchange(expected, desired, weak, Order::kSeqCst, Order::kSeqCst);
    }
    bool CompareExchange(T expected, T desired, bool weak)
    {
      return CompareExchange(&expected, desired, weak);
    }
    
    constexpr bool AlwaysLockFree() { return __atomic_always_lock_free(sizeof (T), 0); }
    bool IsLockFree() { return __atomic_is_lock_free(sizeof (T), 0); }
    
    private:
    T data;
  };
}

