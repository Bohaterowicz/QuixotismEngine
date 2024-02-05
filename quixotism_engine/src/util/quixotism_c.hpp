#pragma once
#include <cassert>
#include <cstdint>
#include <type_traits>

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

template <typename T, size_t N>
inline consteval auto ArraySize(T (&Arr)[N]) {
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
  assert(Value <= 0xFFFFFFFF);
  // Get the 32bit value from the 64bit LARGE_INTEGER
  auto Value32 = static_cast<u32>(Value);
  return Value32;
}

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
