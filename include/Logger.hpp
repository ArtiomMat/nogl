#pragma once

#include "Mutex.hpp"

namespace nogl
{
  class Thread;
  // Thread safe alternative to using std::cout, and also has my own touches.
  class Logger
  {
    friend class Thread;

    private:
    Logger() = default;
    ~Logger() = default;

    static Mutex mutex_;
    
    // Interfaced by `Thread` to determine index of thread.
    // NOTE: Since Thread modifies it, it will never modify main's index, leaving it at 0, perfect.
    static thread_local unsigned index_;

    static Logger logger_;

    public:
    Logger(Logger&) = delete;
    void operator=(const Logger&) = delete;

    enum class Code : int
    {
      kBegin,
      kEnd,
    };

    Logger& PushPtr(void* ptr);
    
    Logger& operator <<(Code c);
    Logger& operator <<(const char* str);
    Logger& operator <<(char c);
    Logger& operator <<(long long i);
    Logger& operator <<(int i) { return (*this) << (long long)i; }
    Logger& operator <<(short i) { return (*this) << (long long)i; }
    Logger& operator <<(unsigned long long i) { return (*this) << (long long)i; }
    Logger& operator <<(unsigned int i) { return (*this) << (long long)i; }
    Logger& operator <<(unsigned short i) { return (*this) << (long long)i; }
    Logger& operator <<(unsigned char i) { return (*this) << (long long)i; }
    template<typename T>
    Logger& operator <<(T* ptr) { return PushPtr(reinterpret_cast<void*>(ptr)); }
    
    // Example: `Logger::Begin() << "Hello " << " world! " << '<' Logger::End();`
    // Locks the internal mutex, so don't hang it for too long... use `End()` to unlock it.
    // If you do need to hang it, prefer writing building some string and writing it at once.
    static Logger& Begin();
    static Code End();
  };
}

