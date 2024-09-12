#include "Logger.hpp"

#include <cstdio>
#include <ctime>

namespace nogl
{
  thread_local unsigned Logger::index_ = 0;
  Mutex Logger::mutex_;
  Logger Logger::logger_;

  Logger& Logger::PushPtr(void* ptr)
  {
    printf("%p", ptr);

    return *this;
  }
  
  Logger& Logger::operator <<(Code c)
  {
    static thread_local char time_buf[24];

    switch (c)
    {
      case Logger::Code::kBegin:
      {
        Logger::mutex_.Lock();

        // Get time
        time_t timer = time(NULL);
        struct tm* tm_info = localtime(&timer);

        strftime(time_buf, sizeof(time_buf), "%d/%m/%Y %H:%M:%S", tm_info);
        
        printf("[%s]{THREAD%u} ", time_buf, index_);
      }
      break;

      case Logger::Code::kEnd:
      putc('\n', stdout);
      
      Logger::mutex_.Unlock();
      break;
    }

    return *this;
  }
  Logger& Logger::operator <<(const char* str)
  {
    fputs(str, stdout);
    return *this;
  }
  Logger& Logger::operator <<(char c)
  {
    putc(c, stdout);
    return *this;
  }
  Logger& Logger::operator <<(long long i)
  {
    printf("%lli", i);
    return *this;
  }
  
  Logger& Logger::Begin()
  {
    return logger_ << Code::kBegin;
  }
  Logger::Code Logger::End()
  {
    return Logger::Code::kEnd;
  }
}
