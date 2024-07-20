#pragma once

#include <queue>

#include "containers/bucket_array.hpp"
#include "core/texture.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

class TextureManager : public BucketArray<Texture> {
 public:
  CLASS_DELETE_COPY(TextureManager);
  TextureManager() = default;

 private:
};
using TextureID = TextureManager::IdType;
}  // namespace quixotism
