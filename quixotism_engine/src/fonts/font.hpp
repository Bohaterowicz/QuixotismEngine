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
  i32 width;
  i32 height;
  i32 baseline_offset;
  r32 h_advance;
};

using GlyphInfoTable = std::vector<GlyphInfo>;

/**
 * IMPORTANT:
 *  Currently 'Font' supports only ASCII characters of range CODEPOINT_START to
 * CODEPOINT_END. This limitation is due to us leveraging the simplicty of using
 * ascii codepoint values as indexes (offset by CODEPOINT_START) into glyph
 * tables.
 *
 * If we want to support other codepoint ranges we would need a more
 * sophisticated way of mapping codepoints to glyphs. This works for now.
 *
 */

class Font {
 public:
  static constexpr u32 CODEPOINT_START = '!';
  static constexpr u32 CODEPOINT_END = '~';
  static constexpr size_t CODEPOINT_COUNT = CODEPOINT_END - CODEPOINT_START + 1;
  CLASS_DELETE_COPY(Font);
  CLASS_DEFAULT_MOVE(Font);
  Font() = default;
  Font(PackedBitmap &&font_bitmap, GlyphInfoTable &&glyph_info, r32 scale,
       i32 ascent, i32 descent, i32 line_gap, i32 space_advance,
       std::unique_ptr<r32[]> &&kerning = nullptr, size_t kerning_size = 0)
      : packed_bitmap{std::move(font_bitmap)},
        glyph_info{std::move(glyph_info)},
        scale{scale},
        ascent{ascent},
        descent{descent},
        line_gap{line_gap},
        space_advance{space_advance},
        kerning_advancment{std::move(kerning)},
        kerning_size{kerning_size} {
    assert((CODEPOINT_COUNT * CODEPOINT_COUNT) == kerning_size);
  };

  const Bitmap &GetBitmap() const { return packed_bitmap.bitmap; }
  const BitmapCoord &GetCodepointBitmapCoord(u32 codepoint) const {
    assert(codepoint >= CODEPOINT_START && codepoint <= CODEPOINT_END);
    return packed_bitmap.coords[codepoint - CODEPOINT_START];
  }
  const GlyphInfo &GetGlyphInfo(u32 codepoint) const {
    assert(codepoint >= CODEPOINT_START && codepoint <= CODEPOINT_END);
    size_t codepoint_idx = codepoint - CODEPOINT_START;
    return glyph_info[codepoint_idx];
  }
  r32 GetScale() const { return scale; }
  i32 GetAscent() const { return ascent; }
  i32 GetDescent() const { return descent; }
  i32 GetLineGap() const { return line_gap; }
  i32 GetSpaceAdvance() const { return space_advance; }

  r32 GetHorizontalAdvance(u32 code_point, u32 prev_code_point) const;

 private:
  PackedBitmap packed_bitmap;
  GlyphInfoTable glyph_info;
  size_t kerning_size;
  // codepoint 2d array of kerning information
  std::unique_ptr<r32[]> kerning_advancment;
  r32 scale;
  i32 ascent, descent, line_gap, space_advance;
};

std::expected<Font, ParseFontError> TTFMakeASCIIFont(const u8 *ttf_data,
                                                     const size_t ttf_size);

}  // namespace quixotism
