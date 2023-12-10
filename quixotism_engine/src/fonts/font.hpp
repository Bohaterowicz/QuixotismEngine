#pragma once

#include <expected>
#include <memory>
#include <vector>

#include "bitmap_processing/bitmap.hpp"
#include "quixotism_c.hpp"
#include "quixotism_error.hpp"

namespace quixotism {

class ParseFontError : public QError {};

struct GlyphCoord {
  r32 x1, y1;
  r32 x2, y2;
};

class Font {
 public:
  Font() = default;
  void InitPackedFont(PackedBitmap &&font_bitmap, i32 _base_codepoint) {
    packed_bitmap = std::move(font_bitmap);
    base_codepoint = _base_codepoint;
  }

  const Bitmap &GetBitmap() const { return packed_bitmap.bitmap; }

 private:
  i32 base_codepoint = 0;
  PackedBitmap packed_bitmap;
};

std::expected<Font, ParseFontError> TTFMakeASCIIFont(const u8 *ttf_data,
                                                     const size_t ttf_size);

}  // namespace quixotism
