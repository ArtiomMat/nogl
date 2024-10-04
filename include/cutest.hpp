#pragma once

#include <vector>
#include <iostream>
#include <cstdlib>
#include <stdexcept>

namespace cutest
{
  struct TestData
  {
    const char* name;
    void (*f)();
  };

  static std::vector<TestData> tests;
  static unsigned i = 0;
  static TestData* current_test = nullptr;

  static void _End()
  {
    unsigned n = tests.size();
    if (i == n)
    {
      std::cout << "\nALL " << n << " TESTS SUCCEEDED!\n" << std::endl;
      // exit(0);
    }
    else
    {
      std::cout << "\nFAILED TEST " << i + 1 << "/" << n << "!\n" << std::endl;
      exit(1);
    }
  }

  // Assert that a condition is true
  static void Assert(bool condition, const char* err = "TEST FAILED.")
  {
    if (!condition)
    {
      std::cerr << "FAILED!\n  MESSAGE: '" << err << "'" << std::endl;
      _End();
    }
  }

  // Assert that a condition is not true
  static inline void AssertNot(bool condition, const char* err = "TEST FAILED.")
  {
    Assert(!condition, err);
  }

  static void Run(unsigned threads = 1)
  {
    i = 0;
    for (auto& t : tests)
    {
      current_test = &t;
      
      std::cout << "\n- TESTING '" << current_test->name << "\'..." << std::flush;
      try {
        t.f();
      } catch (const std::exception& e) {
        std::cerr << "FAILED WITH EXCEPTION!\n  what(): '" << e.what() << "'" << std::endl;
        _End();
      }
      std::cout << "SUCCESS!" << std::endl;

      i++;
    }

    // Success
    _End();
  }

  static void Push(const char* name, void (*f)())
  {
    tests.push_back(TestData{name, f});
  }
}