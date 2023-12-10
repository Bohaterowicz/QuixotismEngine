#include "bitmap_processing/bitmap.hpp"

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
      assert(!"unreachable");
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
      assert(!"unreachable");
  }
}

Bitmap::Bitmap(i32 _width, i32 _height, BitmapFormat _format)
    : width{_width}, height{_height}, format{_format} {
  data = std::make_unique<u8[]>(width * height * BytesPerPixel());
}

std::expected<PackedBitmap, BitmapError> PackBitmaps(
    const std::vector<Bitmap> &bitmaps) {
  constexpr i32 max_bitmap_dimension = 4092;
  Bitmap font_bitmap;
  // first figure out the biggest edge size of all glyphs
  i32 max_sub_bitmap_size = 0;
  for (const auto &bitmap : bitmaps) {
    const auto [width, height] = bitmap.GetDim();
    max_sub_bitmap_size = std::max(max_sub_bitmap_size, width);
    max_sub_bitmap_size = std::max(max_sub_bitmap_size, height);
  }
  // compute number of elements per dimension of the square
  i32 elements_per_dim =
      static_cast<i32>(std::ceilf(std::sqrtf(bitmaps.size())));

  // get the pixel size of the bitmap square using max_sub_bitmap_size
  i32 bitmap_dimension = elements_per_dim * max_sub_bitmap_size;

  // get nearest power of 2 aligned bitmap size
  i32 aligned_bitmap_dimension = 64;
  while (aligned_bitmap_dimension < bitmap_dimension) {
    aligned_bitmap_dimension <<= 1;
  }

  // if that size is bigger than the allowed maximum dimension pixel size,
  // return null
  if (aligned_bitmap_dimension > max_bitmap_dimension) {
    DBG_PRINT(
        "could not create font bitmap: baked texture would be too big (max "
        "font bitmap is 4k)");
    return std::unexpected(BitmapError{});
  }

  PackedBitmap packed;
  packed.bitmap = Bitmap{aligned_bitmap_dimension, aligned_bitmap_dimension,
                         Bitmap::BitmapFormat::R8};
  packed.coords.reserve(bitmaps.size());

  for (size_t idx = 0; idx < bitmaps.size(); ++idx) {
    const auto &sub_bitmap = bitmaps[idx];
    const auto [width, height] = sub_bitmap.GetDim();
    BitmapCoord coord;
    i32 row = idx / elements_per_dim;
    i32 col = idx % elements_per_dim;

    coord.lower_left.x = (row * max_sub_bitmap_size) / aligned_bitmap_dimension;
    coord.lower_left.y = (col * max_sub_bitmap_size) / aligned_bitmap_dimension;
    coord.top_right.x =
        (coord.lower_left.x + max_sub_bitmap_size) / aligned_bitmap_dimension;
    coord.top_right.y =
        (coord.lower_left.y + max_sub_bitmap_size) / aligned_bitmap_dimension;

    auto *source = sub_bitmap.GetBitmapPtr();
    auto *dest_row =
        packed.bitmap.GetBitmapWritePtr() +
        ((aligned_bitmap_dimension - ((row * max_sub_bitmap_size) + 1)) *
         aligned_bitmap_dimension) +
        (col * max_sub_bitmap_size);
    for (i32 y = 0; y < height; ++y) {
      auto *dest = dest_row;
      for (i32 x = 0; x < width; ++x) {
        *dest++ = *source++;
      }
      dest_row -= aligned_bitmap_dimension;
    }
  }

  return packed;
}

}  // namespace quixotism
