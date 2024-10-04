// Easy include module.
#pragma once

#include "Clock.hpp"
#include "Context.hpp"
#include "Chain.hpp"

#include "Logger.hpp"
#include "Atomic.hpp"
#include "Thread.hpp"
#include "Minion.hpp"

#include "Image.hpp"
#include "Font.hpp"
#include "Scene.hpp"

#include "math.hpp"

#ifndef __x86_64__
  #error Need x86_64 architecture.
#endif

// Need 64 bit system...
static_assert(sizeof(void*) == 8);

namespace nogl
{
  void Test();
}
