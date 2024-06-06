#include "gl_texture.hpp"

#include "GL/glew.h"
#include "gl_call.hpp"

namespace quixotism {

void GLTexture::BindUnit(u32 unit) { GLCall(glBindTextureUnit(unit, id)); }

GLTexture CreateTexture(const Bitmap &bitmap) {
  u32 id = 0;
  GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &id));
  auto [width, height] = bitmap.GetDim();
  GLCall(glTextureStorage2D(id, 6, GL_R8, width, height));
  GLCall(glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RED,
                             GL_UNSIGNED_BYTE, bitmap.GetBitmapPtr()));
  GLCall(glGenerateTextureMipmap(id));
  GLTexture texture{id};
  return texture;
}

}  // namespace quixotism
