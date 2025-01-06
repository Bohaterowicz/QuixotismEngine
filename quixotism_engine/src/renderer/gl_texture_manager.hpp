#pragma once
#include "containers/bucket_array.hpp"
#include "gl_texture.hpp"

namespace quixotism {

class GLTextureManager : public BucketArray<GLTexture> {
 public:
  CLASS_DELETE_COPY(GLTextureManager);

  GLTextureManager() {}

  IdType CreateTexture(const TextureDesc &desc) {
    auto tex = quixotism::CreateTexture(desc);
    return Add(std::move(tex));
  }

 private:
};

using GLTextureID = GLTextureManager::IdType;

}  // namespace quixotism
