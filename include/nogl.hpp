// Easy include module.
#pragma once

#include "ClockDriver.hpp"
#include "FileDriver.hpp"
#include "Context.hpp"
#include "Font.hpp"
#include "Thread.hpp"
#include "VOV.hpp"

#ifndef __x86_64__
  #error Need x86_64 architecture.
#endif

// Need 64 bit system...
static_assert(sizeof(void*) == 8);
