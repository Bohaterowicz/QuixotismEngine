#include "win32_quixotism_time.hpp"

#include "Windows.h"

auto Win32GetWorldTimestamp() -> u64 {
  FILETIME time;
  GetSystemTimePreciseAsFileTime(&time);
  return ULARGE_INTEGER{time.dwLowDateTime, time.dwHighDateTime}.QuadPart;
}