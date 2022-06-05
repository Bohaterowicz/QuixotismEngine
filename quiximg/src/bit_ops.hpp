#pragma once
#include "quixotism_c.hpp"

#if COMPILER_MSVC
#include <intrin.h>
#endif

struct bit_scan_result
{
    bool32 Found;
    uint32 Index;
};

inline uint32 BitRotateLeft(uint32 Value, int32 Shift)
{
    uint32 Result = _rotl(Value, Shift);
    return Result;
}

inline bit_scan_result BitScanForward(uint32 Value)
{
    bit_scan_result Result = {};
#if COMPILER_MSVC
    Result.Found = _BitScanForward(reinterpret_cast<unsigned long *>(&Result.Index), Value); // NOLINT
#else
    for (uint32 ShiftIndex = 0; ShiftIndex < 32; ++ShiftIndex)
    {
        if (Value & (1 << ShiftIndex))
        {
            Result.Index = ShiftIndex;
            Result.Found = true;
            break;
        }
    }
#endif
    return Result;
}