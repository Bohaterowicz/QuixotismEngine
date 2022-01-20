#pragma once
#include "quixotism_c.hpp"
#include <Windows.h>

using timestamp = LARGE_INTEGER;

const real32 MILLISECONDS_IN_SECONDS = 1000.0F;

struct seconds
{
    real64 Count;

    static seconds FromSeconds(real64 Val)
    {
        return seconds{Val};
    }
};

struct milliseconds
{
    real64 Count;

    static milliseconds FromSeconds(real64 Val)
    {
        return milliseconds{MILLISECONDS_IN_SECONDS * Val};
    }
};

class win32_timer
{
  public:
    explicit win32_timer()
    {
        // Get the clock frequency
        LARGE_INTEGER LI_ClockFrequency;
        QueryPerformanceFrequency(&LI_ClockFrequency);
        ClockFrequency = LI_ClockFrequency.QuadPart;
    }

    static timestamp GetTimestamp()
    {
        timestamp Result;
        QueryPerformanceCounter(&Result);
        return Result;
    }

    template <typename T> T GetTimeDifference(timestamp Start, timestamp End)
    {
        auto SecondsDiff = (static_cast<real64>(End.QuadPart - Start.QuadPart) / static_cast<real64>(ClockFrequency));
        return T::FromSeconds(SecondsDiff);
    }

  private:
    int64 ClockFrequency = 0;
};
