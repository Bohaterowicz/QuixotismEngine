#pragma once

#include <vector>

#include "gl_texture_manager.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

/*
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_COLOR_ATTACHMENT9 0x8CE9
#define GL_COLOR_ATTACHMENT10 0x8CEA
#define GL_COLOR_ATTACHMENT11 0x8CEB
#define GL_COLOR_ATTACHMENT12 0x8CEC
#define GL_COLOR_ATTACHMENT13 0x8CED
#define GL_COLOR_ATTACHMENT14 0x8CEE
#define GL_COLOR_ATTACHMENT15 0x8CEF
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
*/

enum AttachmentType {
  COLOR0 = 0x8CE0,
  DEPTH = 0x8D00,
  STENCIL = 0x8D20,
  DEPTH_STENCIL = 0x821A,
};

struct FramebufferDesc {
  std::vector<std::pair<AttachmentType, GLTextureID>> attachments;
};

class Framebuffer {
 public:
  CLASS_DELETE_COPY(Framebuffer);
  Framebuffer(Framebuffer &&other)
      : id{std::move(other.id)}, attachments{std::move(other.attachments)} {
    other.id = 0;
  }
  Framebuffer &operator=(Framebuffer &&other) {
    id = std::move(other.id);
    attachments = std::move(other.attachments);
    other.id = 0;
    other.attachments.clear();
    return *this;
  }
  Framebuffer() = default;
  Framebuffer(u32 _id) : id{_id} {}

  ~Framebuffer();

  u32 Id() const { return id; }

  void Bind();

  void AttachTexture(AttachmentType type, GLTextureID tex_id);

  std::vector<std::pair<AttachmentType, GLTextureID>> attachments;

 private:
  u32 id = 0;
};

Framebuffer CreateFramebuffer(const FramebufferDesc &desc);

}  // namespace quixotism