#pragma once
#include <cstdint>

#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif

#if !defined(COMPILER_LLVM)
#define COMPILER_LLVM 0
#endif

#if !COMPILER_MSVC && !COMPILER_LLVM
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#else
#undef COMPILER_LLVM
#define COMPILER_LLVM 1
#endif
#endif

#ifdef BUILD_INTERNAL
#define Assert(Expression)                                                                                             \
    if (!(Expression))                                                                                                 \
    {                                                                                                                  \
        *(int *)0 = 0;                                                                                                 \
    }
#else
#define Assert(Expression)
#endif

#define LOCAL_PERSIST static
#define GLOBAL_VARIABLE static
#define INTERNAL static

#define Kilobytes(X) (X * 1024LL)
#define Megabytes(X) (Kilobytes(X) * 1024LL)
#define Gigabytes(X) (Megabytes(X) * 1024LL)
#define Terabytes(X) (Gigabytes(X) * 1024LL)

#define TRUE 1
#define FALSE 0

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using real32 = float;
using real64 = double;
using bool32 = int32;
using size = size_t;

template <typename T, size N> auto constexpr ArrayCount(T (&Arr)[N])
{
    return N;
}

template <typename T> void constexpr SwapPointers(T *A, T *B)
{
    T *Temp = A;
    A = B;
    B = Temp;
}
