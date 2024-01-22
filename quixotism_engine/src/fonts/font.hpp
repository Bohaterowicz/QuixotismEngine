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
  i32 pixel_width;
  i32 pixel_height;
  i32 pixel_baseline_offset;
  BitmapCoord coord;
};

using GlyphInfoMap = std::unordered_map<u32, GlyphInfo>;

class Font {
 public:
  CLASS_DELETE_COPY(Font);
  CLASS_DEFAULT_MOVE(Font);
  Font() = default;
  Font(Bitmap &&font_bitmap, GlyphInfoMap &&glyph_info, r32 scale, i32 ascent,
       i32 descent, i32 line_gap)
      : bitmap{std::move(font_bitmap)},
        glyph_info{std::move(glyph_info)},
        scale{scale},
        ascent{ascent},
        descent{descent},
        line_gap{line_gap} {};

  const Bitmap &GetBitmap() const { return bitmap; }
  const GlyphInfo &GetGlyphInfo(u32 code_point) const {
    return glyph_info.at(code_point);
  }
  r32 GetScale() const { return scale; }
  i32 GetAscent() const { return ascent; }
  i32 GetDescent() const { return descent; }
  i32 GetLineGap() const { return line_gap; }

  r32 GetHorizontalAdvance(u32 code_point, u32 prev_code_point) const;

 private:
  Bitmap bitmap;
  GlyphInfoMap glyph_info;
  r32 scale;
  i32 ascent, descent, line_gap;
};

std::expected<Font, ParseFontError> TTFMakeASCIIFont(const u8 *ttf_data,
                                                     const size_t ttf_size);

}  // namespace quixotism
