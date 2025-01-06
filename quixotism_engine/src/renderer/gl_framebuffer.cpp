#include "gl_framebuffer.hpp"

#include "GL/glew.h"
#include "gl_call.hpp"
#include "quixotism_renderer.hpp"

namespace quixotism {

Framebuffer::~Framebuffer() { GLCall(glDeleteFramebuffers(1, &id)); }

void Framebuffer::AttachTexture(AttachmentType type, GLTextureID tex_id) {
  auto *gl_tex = QuixotismRenderer::GetRenderer().texture_mgr.Get(tex_id);
  GLCall(glNamedFramebufferTexture(id, type, gl_tex->Id(), 0));
}

void Framebuffer::Bind() {
  GLCall(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GLCall(auto fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
  if (fbo_status != GL_FRAMEBUFFER_COMPLETE) {
    Assert(0);
  }
}

Framebuffer CreateFramebuffer(const FramebufferDesc &desc) {
  u32 id;
  GLCall(glCreateFramebuffers(1, &id));
  Assert(id);
  Framebuffer fbo{id};
  for (const auto &[type, tex_id] : desc.attachments) {
    fbo.AttachTexture(type, tex_id);
  }
  fbo.attachments = desc.attachments;
  return fbo;
}

}  // namespace quixotism
