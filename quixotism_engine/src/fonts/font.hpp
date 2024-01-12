#pragma once

#include <expected>
#include <memory>
#include <vector>
#include <unordered_map>

#include "bitmap_processing/bitmap.hpp"
#include "quixotism_c.hpp"
#include "quixotism_error.hpp"

namespace quixotism {

class ParseFontError : public QError {};

struct GlyphCoord {
  r32 x1, y1;
  r32 x2, y2;
};

struct GlyphInfo {
  i32 bitmap_pixel_width;
  i32 bitmap_pixel_height;
  r32 ratio;
  BitmapCoord coord;
};

class Font {
 public:
  Font() = default;

  const Bitmap &GetBitmap() const { return bitmap; }
  const GlyphInfo &GetGlyphInfo(const char c) const {
    return glyph_info.at(c);
  }
 private:
  Bitmap bitmap;
  std::unordered_map<char, GlyphInfo> glyph_info;
};

std::expected<Font, ParseFontError> TTFMakeASCIIFont(const u8 *ttf_data,
                                                     const size_t ttf_size);

}  // namespace quixotism
