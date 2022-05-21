#pragma once
#include "quixotism_c.hpp"
#include <cmath>

#if COMPILER_MSVC
#include <intrin.h>
#endif

#define Pi32 3.14159265359f

struct bit_scan_result
{
    bool32 Found;
    uint32 Index;
};

inline int32 SignOf(int32 Value)
{
    int32 Result = (Value >= 0 ? 1 : -1);
    return Result;
}

inline real32 AbsoluteValue(real32 Value)
{
    real32 Result = fabsf(Value);
    return Result;
}

inline int32 RoundToInt32(real32 Value)
{
    auto Result = static_cast<int32>(roundf(Value));
    return Result;
}

inline uint32 RoundToUInt32(real32 Value)
{
    auto Result = static_cast<uint32>(roundf(Value));
    return Result;
}

inline uint32 FloorToUInt32(real32 Value)
{
    auto Result = static_cast<uint32>(floorf(Value));
    return Result;
}

inline int32 FloorToInt32(real32 Value)
{
    auto Result = static_cast<int32>(floorf(Value));
    return Result;
}

inline uint32 TruncateToUInt32(real32 Value)
{
    auto Result = static_cast<uint32>(Value);
    return Result;
}

inline int32 TruncateToInt32(real32 Value)
{
    auto Result = static_cast<int32>(Value);
    return Result;
}

inline uint32 BitRotateLeft(uint32 Value, int32 Shift)
{
    uint32 Result = _rotl(Value, Shift);
    return Result;
}

inline uint32 BitRotateRight(uint32 Value, int32 Shift)
{
    uint32 Result = _rotr(Value, Shift);
    return Result;
}

// Safely truncates a 64bit uint into a 32bit uint
inline uint32 SafeTruncateUint64(uint64 Value)
{
    // Assert that the value stored in the uint64 is smaller or equal to a maximum uint32 value, so that we wont lose
    // anythinbg during truncation
    Assert(Value <= 0xFFFFFFFF);
    // Get the 32bit value from the 64bit LARGE_INTEGER
    auto Value32 = static_cast<uint32>(Value);
    return Value32;
}

inline real32 Sin(real32 Angle)
{
    return sinf(Angle);
}

inline real32 Cos(real32 Angle)
{
    return cosf(Angle);
}

inline real32 Atan2(real32 Y, real32 X)
{
    return atan2f(Y, X);
}

inline real32 Squared(real32 A)
{
    return A * A;
}

inline real32 SquareRoot(real32 A)
{
    return sqrtf(A);
}

inline bit_scan_result BitScanForward(uint32 Value)
{
    bit_scan_result Result = {};
#if COMPILER_MSVC
    Result.Found = _BitScanForward(reinterpret_cast<unsigned long *>(&Result.Index), Value);
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