#include "gl_sampler.hpp"

#include "GL/glew.h"
#include "gl_call.hpp"

namespace quixotism {

GLSampler CreateSampler() {
  u32 id;
  GLCall(glCreateSamplers(1, &id));
  GLSampler sampler{id};
  GLCall(glSamplerParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
  GLCall(glSamplerParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
  GLCall(
      glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
  GLCall(glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  return sampler;
}

GLSampler CreateSampler2() {
  u32 id;
  GLCall(glCreateSamplers(1, &id));
  GLSampler sampler{id};
  GLCall(glSamplerParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT));
  GLCall(glSamplerParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT));
  GLCall(
      glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
  GLCall(glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  return sampler;
}

}  // namespace quixotism
