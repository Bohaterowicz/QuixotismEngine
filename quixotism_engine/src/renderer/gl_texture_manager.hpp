#pragma once
#include "containers/bucket_array.hpp"
#include "gl_texture.hpp"

namespace quixotism {

using GLTextureID = u32;

class GLTextureManager : public BucketArray<GLTexture> {
 public:
  CLASS_DELETE_COPY(GLTextureManager);

  GLTextureManager() {}

 private:
};

}  // namespace quixotism
