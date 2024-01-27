#pragma once

#include "quixotism_c.hpp"

namespace quixotism {

enum BufferDataMode : u32 {
  STREAM_DRAW = 0x88E0,
  STREAM_READ = 0x88E1,
  STREAM_COPY = 0x88E2,
  STATIC_DRAW = 0x88E4,
  STATIC_READ = 0x88E5,
  STATIC_COPY = 0x88E6,
  DYNAMIC_DRAW = 0x88E8,
  DYNAMIC_READ = 0x88E9,
  DYNAMIC_COPY = 0x88EA
};

struct GLBuffer {
  static constexpr u32 INVALID_BUFFER_ID = 0;
  u32 id = INVALID_BUFFER_ID;
};

bool GLBufferData(GLBuffer &buffer, void *data, size_t size,
                  BufferDataMode mode);

}  // namespace quixotism
