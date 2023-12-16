#pragma once

#include <expected>
#include <memory>
#include <vector>

#include "math/vector.hpp"
#include "quixotism_c.hpp"
#include "quixotism_error.hpp"

namespace quixotism {

class BitmapError : public QError {};

class Bitmap {
 public:
  enum class BitmapFormat : u8 {
    UNSPECIFIED,
    R8 = 1,
    RGB8 = 2,
    RGBA8 = 3,
  };

  static i32 FormatChannelCount(BitmapFormat format);
  static i32 FormatBytesPerPixel(BitmapFormat format);

 public:
  Bitmap() : width{0}, height{0}, format{BitmapFormat::UNSPECIFIED} {}
  explicit Bitmap(i32 _width, i32 _height, BitmapFormat _format);

  Bitmap(const Bitmap &) = delete;
  Bitmap &operator=(const Bitmap &) = delete;

  Bitmap(Bitmap &&other) = default;
  Bitmap &operator=(Bitmap &&) = default;

  bool IsValid() const { return data != nullptr; }

  i32 BytesPerPixel() const { return Bitmap::FormatBytesPerPixel(format); }
  const u8 *GetBitmapPtr() const { return data.get(); }
  u8 *GetBitmapWritePtr() { return data.get(); }

  std::pair<i32, i32> GetDim() const { return {width, height}; }
  i32 GetWidth() const { return width; }
  i32 GetHeight() const {return height; }

 private:
  i32 width;
  i32 height;
  BitmapFormat format;
  std::unique_ptr<u8[]> data;
};

struct BitmapCoord {
  Vec2 lower_left;
  Vec2 top_right;
};

struct PackedBitmap {
  Bitmap bitmap;
  std::vector<BitmapCoord> coords;
};

std::expected<PackedBitmap, BitmapError> PackBitmaps(
    const std::vector<Bitmap> &bitmaps, const i32 padding = 2, const i32 max_bitmap_dim = 4096);

}  // namespace quixotism
