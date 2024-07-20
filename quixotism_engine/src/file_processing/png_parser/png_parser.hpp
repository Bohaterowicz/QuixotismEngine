#pragma once
#include <expected>

#include "bitmap/bitmap.hpp"
#include "quixotism_c.hpp"

namespace quixotism {
static u8 png_signature[] = {137, 80, 78, 71, 13, 10, 26, 10};

#pragma pack(push, 1)
struct PNGHeader {
  u8 signature[8];
};

struct PNGChunkHeader {
  u32 length;
  union {
    u32 type_u32;
    char type[4];
  };
};

struct PNGChunkFooter {
  u32 CRC;
};

struct PNGIHDR {
  u32 width;
  u32 height;
  u8 bit_depth;
  u8 color_type;
  u8 compression_method;
  u8 filter_method;
  u8 interlace_method;
};

struct ZLIBHeader {
  union {
    struct {
      u8 zlib_method_flags;
      u8 additional_flags;
    };
    struct {
      u8 cm : 4;
      u8 cinfo : 4;
      u8 fcheck : 5;
      u8 fdict : 1;
      u8 flevel : 2;
    };
  };
};
static_assert(sizeof(ZLIBHeader) == 2);

struct PNGIDATFooter {
  u8 check_value[4];
};
#pragma pack(pop)

std::expected<Bitmap, BitmapError> ParsePNG(void *data, size_t size);

}  // namespace quixotism
