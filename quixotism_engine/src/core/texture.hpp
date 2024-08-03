#pragma once

#include <variant>

#include "bitmap/bitmap.hpp"
#include "quixotism_c.hpp"
#include "renderer/gl_texture.hpp"

namespace quixotism {

class Texture {
 public:
  Texture() = default;
  Texture(Bitmap &&_bitmap) : bitmap{std::move(_bitmap)}, is_cube{false} {}
  Texture(CubeBitmap &&_bitmap) : bitmap{std::move(_bitmap)}, is_cube{true} {}

  bool is_cube = false;
  std::variant<Bitmap, CubeBitmap> bitmap;
  GLTexture glid;

 private:
};

}  // namespace quixotism
