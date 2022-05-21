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
    static int64 ClockFrequency;
    static timestamp InitTimestamp;
    static int64 __InitQuixotismClock()
    {
        // Get the clock frequency
        LARGE_INTEGER LI_ClockFrequency;
        QueryPerformanceFrequency(&LI_ClockFrequency);
        return LI_ClockFrequency.QuadPart;
    }

    static timestamp GetTimestamp()
    {
        timestamp Result;
        QueryPerformanceCounter(&Result);
        return Result;
    }

    template <typename T> static T GetTimeDifference(timestamp Start, timestamp End)
    {
        auto SecondsDiff = (static_cast<real64>(End.QuadPart - Start.QuadPart) / static_cast<real64>(ClockFrequency));
        return T::FromSeconds(SecondsDiff);
    }
};
int64 win32_timer::ClockFrequency = win32_timer::__InitQuixotismClock();
timestamp win32_timer::InitTimestamp = win32_timer::GetTimestamp();

real64 GetRunningTime()
{
    auto CurrentTimestamp = win32_timer::GetTimestamp();
    auto Result = win32_timer::GetTimeDifference<seconds>(win32_timer::InitTimestamp, CurrentTimestamp);
    return Result.Count;
}