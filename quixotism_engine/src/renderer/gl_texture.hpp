#pragma once
#include "bitmap/bitmap.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

enum TextureType {
  TEXTURE_2D = 0x0DE1,
  TEXTURE_3D = 0x806F,
  TEXTURE_CUBE_MAP = 0x8513,
  TEXTURE_2D_ARRAY = 0x8C1A,
};

struct TextureDesc {
  TextureType type;
  i32 width;
  i32 height;
  BitmapFormat format;
  u32 count;
  u32 mip_levels;
};

class GLTexture {
 public:
  CLASS_DELETE_COPY(GLTexture);
  GLTexture(GLTexture &&other) : id{std::move(other.id)} { other.id = 0; }
  GLTexture &operator=(GLTexture &&other) {
    id = other.id;
    other.id = 0;
    return *this;
  }
  GLTexture() = default;
  GLTexture(u32 p_id) : id(p_id) {}

  ~GLTexture();

  u32 Id() const { return id; }

  void BindUnit(u32 unit);

 private:
  u32 id;
};

GLTexture CreateTexture(const TextureDesc &desc);
GLTexture CreateTexture2D(const Bitmap &bitmap);
GLTexture CreateTexture(const Bitmap &bitmap, bool r8);
GLTexture CreateTextureArray(const Bitmap **bitmap, size_t count, bool r8);
GLTexture CreateCubeTexture(std::array<Bitmap, 6> &bitmaps);

}  // namespace quixotism
