#include "Logger.hpp"
#include "Clock.hpp"

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
    // All below variables are protected by mutex

    static thread_local char time_buf[24];
    // Index of last logger. A large number to avoid the logger thinking on the first log that it was this thread last.
    static unsigned last_index = ~0;

    switch (c)
    {
      case Logger::Code::kBegin:
      {
        Logger::mutex_.Lock();

        // Get time
        time_t timer = time(nullptr);
        struct tm* tm_info = localtime(&timer);
        // unsigned t = Clock::global_now();
        // tm_info->tm_sec = (t / 1000) % 60;
        // tm_info->tm_min = (t / (1000 * 60)) % 60;
        // tm_info->tm_hour = (t / (1000 * 60 * 60)) % 24;
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

        // Only print the thread if it's a new logger.
        if (last_index != Logger::index_)
        {
          printf("TH%u\t[%s] ", index_, time_buf);
          last_index = Logger::index_;
        }
        else
        {
          printf("\t[%s] ", time_buf);
        }
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
  Logger& Logger::operator <<(const std::string& str)
  {
    fputs(str.c_str(), stdout);
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
  Logger& Logger::operator <<(double d)
  {
    printf("%lf", d);
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
