#pragma once
#include <print>

#ifndef NDEBUG
#define DBG_PRINT(msg) std::print(stdout, "DEBUG: {}\n", msg);
#else
#define DBG_PRINT(msg)
#endif