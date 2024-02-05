#pragma once
#include <memory>
#include <string>
#include <vector>

#include "gl_buffer_manager.hpp"
#include "math/math.hpp"
#include "quixotism_c.hpp"
#include "shader_manager.hpp"
#include "vertex_array_manager.hpp"

namespace quixotism {

struct TextDrawInfo {
  std::string text;
  Vec2 position;
  r32 scale;
};

class QuixotismRenderer {
 public:
  CLASS_DELETE_COPY(QuixotismRenderer);

  static QuixotismRenderer& GetRenderer() {
    static QuixotismRenderer renderer{};
    return renderer;
  }

  void ClearRenderTarget();

  void Test();

  void PushText(std::string&& text, Vec2 position, r32 scale);

  VertexArrayManager vertex_array_mgr;
  GLBufferManager gl_buffer_mgr;

  VertexArrayID vao = 0, text_vao = 0;
  GLBufferID vbo = 0, ebo = 0, vbo2, ebo2, text_vbo_id = 0;
  ShaderID shader_id, shader_id2, font_shader_id;
  unsigned int texture1;

 private:
  QuixotismRenderer();

  void CompileTextShader();

  void DrawText();
  std::vector<TextDrawInfo> draw_text_queue;

  std::unique_ptr<u8[]> cached_text_vert_buffer;
  size_t cached_text_vert_buffer_size = 0;

  ShaderManager shader_mgr;
};

}  // namespace quixotism
