#include "fonts/font.hpp"

#include <cmath>
#include <expected>

#include "dbg_print.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

namespace quixotism {

static std::expected<Bitmap, ParseFontError> GetGlyphFromFont(
    stbtt_fontinfo *font, char codepoint, float scale) {
  i32 width, height, x_offset, y_offset;
  auto stbtt_bitmap = stbtt_GetCodepointBitmap(
      font, 0, scale, codepoint, &width, &height, &x_offset, &y_offset);

  if (stbtt_bitmap == nullptr) {
    return std::unexpected(ParseFontError{});
  }

  Bitmap glyph{width, height, Bitmap::BitmapFormat::R8};
  u8 *source = stbtt_bitmap;
  u8 *dest_row = glyph.GetBitmapWritePtr() + ((height - 1) * width);
  for (i32 y = 0; y < height; ++y) {
    auto *dest = dest_row;
    for (i32 x = 0; x < width; ++x) {
      *dest++ = *source++;
    }
    dest_row -= width;
  }
  stbtt_FreeBitmap(stbtt_bitmap, nullptr);

  return glyph;
}

std::expected<Font, ParseFontError> TTFMakeASCIIFont(const u8 *ttf_data,
                                                     const size_t ttf_size) {
  stbtt_fontinfo font_info;
  stbtt_InitFont(&font_info, ttf_data, 0);
  constexpr char codepoint_start = '!';
  constexpr char codepoint_end = '~';
  constexpr char codepoint_count = codepoint_end - codepoint_start + 1;
  auto font_pixel_scale = stbtt_ScaleForPixelHeight(&font_info, 180.0);
  std::vector<Bitmap> glyphs;
  glyphs.reserve(codepoint_count);
  for (char codepoint_idx = 0; codepoint_idx < codepoint_count;
       ++codepoint_idx) {
    char codepoint = codepoint_start + codepoint_idx;
    auto glyph = GetGlyphFromFont(&font_info, codepoint, font_pixel_scale);
    if (glyph.has_value()) {
      glyphs.push_back(std::move(*glyph));
    } else {
      DBG_PRINT("aa");
      return std::unexpected(ParseFontError{});
    }
  }
  std::vector<GlyphCoord> glyph_coords{codepoint_count};
  Font font;
  auto packed_font_bitmap = PackBitmaps(glyphs);
  if (!packed_font_bitmap.has_value()) {
    return std::unexpected(ParseFontError{});
  }
  font.InitPackedFont(std::move(*packed_font_bitmap), codepoint_start);
  return font;
}

}  // namespace quixotism
