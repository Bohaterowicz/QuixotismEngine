#include "gl_texture.hpp"

#include "GL/glew.h"
#include "gl_call.hpp"

namespace quixotism {

void GLTexture::BindUnit(u32 unit) { GLCall(glBindTextureUnit(unit, id)); }

GLTexture CreateTexture(const Bitmap &bitmap, bool r8) {
  u32 id = 0;
  GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &id));
  auto [width, height] = bitmap.GetDim();
  auto internal_format = (r8) ? GL_R8 : GL_RGBA8;
  auto format = (r8) ? GL_RED : GL_RGBA;
  GLCall(glTextureStorage2D(id, 6, internal_format, width, height));
  GLCall(glTextureSubImage2D(id, 0, 0, 0, width, height, format,
                             GL_UNSIGNED_BYTE, bitmap.GetBitmapPtr()));
  GLCall(glGenerateTextureMipmap(id));
  GLTexture texture{id};
  return texture;
}

GLTexture CreateTextureArray(const Bitmap **bitmap, size_t count, bool r8) {
  u32 id = 0;
  GLCall(glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &id));
  auto [width, height] = bitmap[count - 1]->GetDim();
  auto internal_format = (r8) ? GL_R8 : GL_RGBA8;
  auto format = (r8) ? GL_RED : GL_RGBA;
  GLCall(glTextureStorage3D(id, 1, internal_format, width, height, count));
  for (u32 idx = 0; idx < count; ++idx) {
    Assert(bitmap[idx]->GetWidth() == width &&
           bitmap[idx]->GetHeight() == height);
    GLCall(glTextureSubImage3D(id, 0, 0, 0, idx, width, height, 1, format,
                               GL_UNSIGNED_BYTE, bitmap[idx]->GetBitmapPtr()));
  }
  GLTexture texture{id};
  return texture;
}

}  // namespace quixotism
