#pragma once
#include "bitmap/bitmap.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

class GLTexture {
 public:
  CLASS_DELETE_COPY(GLTexture);
  CLASS_DEFAULT_MOVE(GLTexture);
  GLTexture() = default;
  GLTexture(u32 p_id) : id(p_id) {}

  u32 Id() const { return id; }

  void BindUnit(u32 unit);

 private:
  u32 id;
};

GLTexture CreateTexture(const Bitmap &bitmap, bool r8);

}  // namespace quixotism
