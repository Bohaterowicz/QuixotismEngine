#pragma once
#if COMPILER_MSVC
#include <stdlib.h>
#endif

#include "quixotism_c.hpp"

namespace quixotism {

void ByteSwap16(u16& v) {
#if COMPILER_MSVC
  v = _byteswap_ushort(v);
#else
  v = (v >> 8) | (v << 8);
#endif
}

void ByteSwap32(u32& v) {
#if COMPILER_MSVC
  v = _byteswap_ulong(v);
#else
  v = (v >> 24) | ((v << 8) & 0x00FF0000) | ((v >> 8) & 0x0000FF00) | (v << 24);
#endif
}

void ByteSwap64(u64& v) {
#if COMPILER_MSVC
  v = _byteswap_uint64(v);
#else
  v = (v >> 56) | ((v << 40) & 0x00FF000000000000) |
      ((v << 24) & 0x0000FF0000000000) | ((v << 8) & 0x000000FF00000000) |
      ((v >> 8) & 0x00000000FF000000) | ((v >> 24) & 0x0000000000FF0000) |
      ((v >> 40) & 0x000000000000FF00) | (v << 56);
#endif
}

}  // namespace quixotism