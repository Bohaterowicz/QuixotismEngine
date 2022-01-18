#pragma once
#include <cstdio>

#if BUILD_INTERNAL
#define DEBUG_OUT(msg) printf_s("DEBUG: %s\n", msg); // std::cout << "DEBUG: " << msg << std::endl;
#else
#define DEBUG_OUT(msg)
#endif