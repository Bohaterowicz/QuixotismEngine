#include "bitmap/bitmap.hpp"

#include <algorithm>
#include <numeric>

#include "dbg_print.hpp"

namespace quixotism {

i32 Bitmap::FormatChannelCount(BitmapFormat format) {
  switch (format) {
    case BitmapFormat::R8:
      return 1;
    case BitmapFormat::RGB8:
      return 3;
    case BitmapFormat::RGBA8:
      return 4;
    case BitmapFormat::UNSPECIFIED:
      return 0;
    default:
      Assert(!"unreachable");
  }
}

i32 Bitmap::FormatBytesPerPixel(BitmapFormat format) {
  switch (format) {
    case BitmapFormat::R8:
      return 1;
    case BitmapFormat::RGB8:
      return 3;
    case BitmapFormat::RGBA8:
      return 4;
    case BitmapFormat::UNSPECIFIED:
      return 0;
    default:
      Assert(!"unreachable");
  }
}

Bitmap::Bitmap(u32 _width, u32 _height, BitmapFormat _format)
    : width{_width}, height{_height}, format{_format} {
  data = std::make_unique<u8[]>(width * height * BytesPerPixel());
}

[[no_discard]] std::expected<PackedBitmap, BitmapError> PackBitmaps(
    const std::vector<Bitmap> &bitmaps, const i32 padding,
    const i32 max_bitmap_dim) {
  if (bitmaps.empty()) return std::unexpected(BitmapError{});

  std::vector<size_t> bitmap_indicies(bitmaps.size());
  std::iota(bitmap_indicies.begin(), bitmap_indicies.end(), 0);
  std::sort(bitmap_indicies.begin(), bitmap_indicies.end(),
            [&](const size_t a, const size_t b) {
              return bitmaps[a].GetHeight() > bitmaps[b].GetHeight();
            });

  // first compute what bitmap size we need to pack all bitmaps
  u32 current_bitmap_dimension = 256;
  while (current_bitmap_dimension <= max_bitmap_dim) {
    bool finished = true;
    u32 current_height =
            bitmaps[bitmap_indicies[0]].GetHeight() + (2 * padding),
        current_width = 0;
    if (current_height > current_bitmap_dimension) {
      current_bitmap_dimension <<= 1;
      continue;
    }
    u32 prev_row_height = current_height;
    for (auto idx : bitmap_indicies) {
      const auto [width, height] = bitmaps[idx].GetDim();
      current_width += width + (2 * padding);
      if (current_width > current_bitmap_dimension) {
        current_height += prev_row_height;
        current_width = width + (2 * padding);
        prev_row_height = height + (2 * padding);
        if ((current_height > current_bitmap_dimension) ||
            (current_width > current_bitmap_dimension)) {
          finished = false;
          break;
        }
      }
    }
    if (finished) break;
    current_bitmap_dimension <<= 1;
  }

  // if that size is bigger than the allowed maximum dimension pixel size,
  // return null
  if (current_bitmap_dimension > max_bitmap_dim) {
    DBG_PRINT(
        "could not create font bitmap: baked texture would be too big (max "
        "font bitmap is 4k)");
    return std::unexpected(BitmapError{});
  }

  PackedBitmap packed_font;
  packed_font.bitmap =
      Bitmap{current_bitmap_dimension, current_bitmap_dimension,
             Bitmap::BitmapFormat::R8};
  packed_font.coords.resize(bitmaps.size());

  u32 current_height = padding, current_width = 0;
  u32 prev_row_height = bitmaps[bitmap_indicies[0]].GetHeight();
  for (const auto idx : bitmap_indicies) {
    const auto &bitmap = bitmaps[idx];
    const auto [width, height] = bitmap.GetDim();
    if ((current_width + width + (2 * padding)) > current_bitmap_dimension) {
      current_width = 0;
      current_height += (prev_row_height + (2 * padding));
      prev_row_height = height;
      Assert(current_width <= current_bitmap_dimension);
      Assert(current_height <= current_bitmap_dimension);
    }
    current_width += padding;
    BitmapCoord coord;
    coord.lower_left = Vec2{static_cast<r32>(current_width),
                            static_cast<r32>(current_height)} /
                       static_cast<r32>(current_bitmap_dimension);
    coord.top_right = Vec2{static_cast<r32>(current_width + width),
                           static_cast<r32>(current_height + height)} /
                      static_cast<r32>(current_bitmap_dimension);
    packed_font.coords[idx] = coord;
    const auto *source = bitmap.GetBitmapPtr();
    auto *dest_row = packed_font.bitmap.GetBitmapWritePtr() +
                     (current_height * current_bitmap_dimension) +
                     (current_width);
    for (i32 y = 0; y < height; ++y) {
      auto *dest = dest_row;
      for (i32 x = 0; x < width; ++x) {
        *dest++ = *source++;
      }
      dest_row += current_bitmap_dimension;
    }
    current_width += (width + padding);
  }

  return packed_font;
}

void CopyBitmap(const Bitmap &src_bitmap, Bitmap &dst_bitmap, u32 dst_xoffset,
                u32 dst_yoffset) {
  if (dst_xoffset || dst_yoffset) {
    Assert(0);
  }

  Assert(src_bitmap.GetWidth() <= dst_bitmap.GetWidth() &&
         src_bitmap.GetHeight() <= dst_bitmap.GetHeight());
  Assert(src_bitmap.BytesPerPixel() == dst_bitmap.BytesPerPixel());

  auto [copy_width, copy_height] = src_bitmap.GetDim();
  size_t copy_size = src_bitmap.BytesPerPixel();
  size_t bytes_per_row = copy_size * dst_bitmap.GetWidth();
  auto src = src_bitmap.GetBitmapPtr();
  auto dst = dst_bitmap.GetBitmapWritePtr();
  auto row = dst;
  for (u32 y = 0; y < copy_height; ++y) {
    auto pixel = row;
    for (u32 x = 0; x < copy_width; ++x) {
      std::memcpy(pixel, src, copy_size);
      src += copy_size;
      pixel += copy_size;
    }
    row += bytes_per_row;
  }
}

}  // namespace quixotism
