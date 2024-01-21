#pragma once

#include <expected>
#include <memory>
#include <unordered_map>
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

struct GlyphInfo {
  i32 bitmap_pixel_width;
  i32 bitmap_pixel_height;
  r32 ratio;
  BitmapCoord coord;
};

using GlyphInfoMap = std::unordered_map<char, GlyphInfo>;

class Font {
 public:
  CLASS_DELETE_COPY(Font);
  CLASS_DEFAULT_MOVE(Font);
  Font() = default;
  Font(Bitmap &&font_bitmap, GlyphInfoMap glyph_info)
      : bitmap{std::move(font_bitmap)}, glyph_info{glyph_info} {};

  const Bitmap &GetBitmap() const { return bitmap; }
  const GlyphInfo &GetGlyphInfo(const char c) const { return glyph_info.at(c); }

 private:
  Bitmap bitmap;
  GlyphInfoMap glyph_info;
};

std::expected<Font, ParseFontError> TTFMakeASCIIFont(const u8 *ttf_data,
                                                     const size_t ttf_size);

}  // namespace quixotism
