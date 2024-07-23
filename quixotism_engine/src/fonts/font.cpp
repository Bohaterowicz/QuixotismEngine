#include "fonts/font.hpp"

#include <cmath>
#include <expected>

#include "dbg_print.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

namespace quixotism {

static std::expected<std::pair<Bitmap, i32>, ParseFontError> GetGlyphFromFont(
    stbtt_fontinfo *font, char codepoint, float scale) {
  i32 width, height, x_offset, y_offset;
  auto stbtt_bitmap = stbtt_GetCodepointBitmap(
      font, 0, scale, codepoint, &width, &height, &x_offset, &y_offset);

  if (stbtt_bitmap == nullptr) {
    return std::unexpected(ParseFontError{});
  }

  Bitmap glyph{(u32)width, (u32)height, Bitmap::BitmapFormat::R8};
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

  return std::make_pair(std::move(glyph), y_offset);
}

std::expected<Font, ParseFontError> TTFMakeASCIIFont(const u8 *ttf_data,
                                                     const size_t ttf_size) {
  stbtt_fontinfo font_info{};
  stbtt_InitFont(&font_info, ttf_data, 0);
  auto font_scale = stbtt_ScaleForPixelHeight(&font_info, 80.0);
  std::vector<Bitmap> glyphs;
  GlyphInfoTable glyph_info_table;
  glyphs.reserve(Font::CODEPOINT_COUNT);
  glyph_info_table.reserve(Font::CODEPOINT_COUNT);
  std::unique_ptr<r32[]> kerning_table = nullptr;
  size_t kerning_size = Font::CODEPOINT_COUNT * Font::CODEPOINT_COUNT;
  if (font_info.kern || font_info.gpos) {
    kerning_table = std::make_unique<r32[]>(kerning_size);
  }

  i32 advance, lsb;
  for (char codepoint_idx = 0; codepoint_idx < Font::CODEPOINT_COUNT;
       ++codepoint_idx) {
    char codepoint = Font::CODEPOINT_START + codepoint_idx;
    auto font_glyph = GetGlyphFromFont(&font_info, codepoint, font_scale);
    if (font_glyph.has_value()) {
      glyphs.push_back(std::move((*font_glyph).first));
      stbtt_GetCodepointHMetrics(&font_info, codepoint, &advance, &lsb);
      if (kerning_table) {
        for (u32 other_codepoint_idx = 0;
             other_codepoint_idx < Font::CODEPOINT_COUNT;
             ++other_codepoint_idx) {
          u32 other_codepoint = Font::CODEPOINT_START + other_codepoint_idx;
          auto kerning_advance = stbtt_GetCodepointKernAdvance(
              &font_info, codepoint, other_codepoint);
          kerning_table[codepoint_idx * Font::CODEPOINT_COUNT +
                        other_codepoint_idx] =
              (font_scale * (kerning_advance + advance));
        }
      }

      glyph_info_table.emplace_back(glyphs.back().GetWidth(),
                                    glyphs.back().GetHeight(),
                                    (*font_glyph).second, advance * font_scale);
    } else {
      return std::unexpected(ParseFontError{});
    }
  }

  i32 space_advance;
  stbtt_GetCodepointHMetrics(&font_info, ' ', &space_advance, &lsb);

  i32 ascent, descent, line_gap;
  stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);

  auto packed_bitmap = PackBitmaps(glyphs);
  if (!packed_bitmap) {
    return std::unexpected(ParseFontError{});
  }

  auto px_scale = stbtt_ScaleForPixelHeight(&font_info, 1.0);

  return Font{std::move(*packed_bitmap),
              std::move(glyph_info_table),
              font_scale,
              ascent,
              descent,
              line_gap,
              space_advance,
              px_scale,
              std::move(kerning_table),
              kerning_size};
}

r32 Font::GetHorizontalAdvance(u32 code_point, u32 prev_code_point) const {
  Assert(code_point >= CODEPOINT_START && code_point <= CODEPOINT_END);
  if (!prev_code_point) {
    return 0;
  }

  Assert(prev_code_point >= CODEPOINT_START &&
         prev_code_point <= CODEPOINT_END);
  size_t code_point_idx = code_point - CODEPOINT_START;
  size_t prev_code_point_idx = prev_code_point - CODEPOINT_START;

  // if we got no kerning information return h_advance form glyph
  if (!kerning_advancment) {
    return glyph_info[prev_code_point_idx].h_advance;
  } else {
    return kerning_advancment[prev_code_point_idx * CODEPOINT_COUNT +
                              code_point_idx];
  }
}

}  // namespace quixotism
