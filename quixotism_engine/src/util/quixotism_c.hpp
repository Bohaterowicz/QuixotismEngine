#pragma once
#include <cstdint>
#include <type_traits>

#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif

#if !defined(COMPILER_GCC)
#define COMPILER_GCC 0
#endif

#if !COMPILER_MSVC && !COMPILER_GCC
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#elif __GNUC__
#undef COMPILER_GCC
#define COMPILER_GCC 1
#else
#error Unsupported (or unrecognized) compiler error...
#endif
#endif

#if COMPILER_MSVC
#define FORCE_INLINE inline __forceinline
#elif COMPILER_GCC
#define FORCE_INLINE inline __attribute__((always_inline))
#else
#error Unsupported (or unrecognized) compiler error...
#endif

#ifndef NDEBUG
#define Assert(Expression) \
  if (!(Expression)) {     \
    *(int *)0 = 0;         \
  }
#else
#define Assert(Expression)
#endif

#define Kilobytes(X) (X * 1024LL)
#define Megabytes(X) (Kilobytes(X) * 1024LL)
#define Gigabytes(X) (Megabytes(X) * 1024LL)
#define Terabytes(X) (Gigabytes(X) * 1024LL)

#define TRUE 1
#define FALSE 0

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using r32 = float;
using r64 = double;

#if defined(DEFAULT_HIGH_PRECISION)
using DefaultMathPrecisionType = r64;
#else
using DefaultMathPrecisionType = r32;
#endif

namespace quixotism {

template <typename T, size_t N>
inline consteval auto ArrayCount(T (&Arr)[N]) {
  return N;
}

template <typename T>
inline constexpr void SwapPointers(T *A, T *B) {
  T *C = A;
  A = B;
  B = C;
}

template <typename E>
constexpr typename std::underlying_type<E>::type EnumValue(E e) noexcept {
  return static_cast<typename std::underlying_type<E>::type>(e);
}

// Safely truncates a 64bit uint into a 32bit uint
inline u32 SafeU64ToU32(u64 Value) {
  // Assert that the value stored in the uint64 is smaller or equal to a maximum
  // uint32 value, so that we wont lose anythinbg during truncation
  Assert(Value <= 0xFFFFFFFF);
  // Get the 32bit value from the 64bit LARGE_INTEGER
  auto Value32 = static_cast<u32>(Value);
  return Value32;
}

inline constexpr u32 FOURCC(const char *String) {
  return (((static_cast<u32>(String[0])) << 0) |
          ((static_cast<u32>(String[1])) << 8) |
          ((static_cast<u32>(String[2])) << 16) |
          ((static_cast<u32>(String[3])) << 24));
}

template <size_t curr, size_t max>
FORCE_INLINE constexpr void Unroll(auto &&f) {
  if constexpr (curr < max) {
    f.template operator()<curr>();
    Unroll<curr + 1, max>(f);
  }
}

}  // namespace quixotism

#define CLASS_DELETE_COPY_CTOR(class) class(const class &) = delete;
#define CLASS_DELETE_MOVE_CTOR(class) class(class &&) = delete;

#define CLASS_DELETE_COPY_ASSIGNMENT(class) \
  class &operator=(const class &) = delete;
#define CLASS_DELETE_MOVE_ASSIGNMENT(class) class &operator=(class &&) = delete;

#define CLASS_DELETE_COPY(class) \
  CLASS_DELETE_COPY_CTOR(class) CLASS_DELETE_COPY_ASSIGNMENT(class);

#define CLASS_DELETE_MOVE(class) \
  CLASS_DELETE_MOVE_CTOR(class) CLASS_DELETE_MOVE_ASSIGNMENT(class)

#define CLASS_DEFAULT_COPY_CTOR(class) class(const class &) = default;
#define CLASS_DEFAULT_MOVE_CTOR(class) class(class &&) = default;

#define CLASS_DEFAULT_COPY_ASSIGNMENT(class) \
  class &operator=(const class &) = default;
#define CLASS_DEFAULT_MOVE_ASSIGNMENT(class) \
  class &operator=(class &&) = default;

#define CLASS_DEFAULT_COPY(class) \
  CLASS_DEFAULT_COPY_CTOR(class) CLASS_DEFAULT_COPY_ASSIGNMENT(class)
#define CLASS_DEFAULT_MOVE(class) \
  CLASS_DEFAULT_MOVE_CTOR(class) CLASS_DEFAULT_MOVE_ASSIGNMENT(class)

#define CLASS_DEFAULT_MOVE_AND_COPY(class) \
  CLASS_DEFAULT_MOVE(class) CLASS_DEFAULT_COPY(class)
