#pragma once
#include "containers/bucket_array.hpp"
#include "gl_framebuffer.hpp"

namespace quixotism {

class FramebufferManager : public BucketArray<Framebuffer> {
 public:
  CLASS_DELETE_COPY(FramebufferManager);
  FramebufferManager() = default;

 private:
};
using FramebufferId = FramebufferManager::IdType;

}  // namespace quixotism
