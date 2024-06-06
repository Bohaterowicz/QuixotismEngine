#pragma once
#include "containers/bucket_array.hpp"
#include "gl_sampler.hpp"

namespace quixotism {

using SamplerID = u32;

class GLSamplerManager : public BucketArray<GLSampler> {
 public:
  CLASS_DELETE_COPY(GLSamplerManager);

  GLSamplerManager() {}

 private:
};

}  // namespace quixotism
