#pragma once

#include "bitmap/bitmap.hpp"
#include "quixotism_c.hpp"
#include "renderer/gl_texture.hpp"

namespace quixotism {

class Texture {
 public:
  Texture() = default;
  Texture(Bitmap &&_bitmap) : bitmap{std::move(_bitmap)} {}

  Bitmap bitmap;
  GLTexture glid;

 private:
};

}  // namespace quixotism
