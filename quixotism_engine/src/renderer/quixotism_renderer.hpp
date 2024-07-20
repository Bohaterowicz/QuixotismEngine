#pragma once
#include <memory>
#include <string>
#include <vector>

#include "core/material_manager.hpp"
#include "core/static_mesh_manager.hpp"
#include "core/transform.hpp"
#include "gl_buffer_manager.hpp"
#include "gl_sampler_manager.hpp"
#include "gl_texture.hpp"
#include "gl_texture_manager.hpp"
#include "math/qmath.hpp"
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

  void PushText(std::string&& text, Vec2 position, r32 scale);
  void DrawText();

  void PrepareDrawStaticMeshes();
  void DrawStaticMesh(const StaticMeshId sm_id, const MaterialID mat_id,
                      const Transform& transform);

  void MakeDrawableStaticMesh(StaticMeshId id);
  void MakeDrawableStaticMesh2(StaticMeshId id, VertexArrayID vao_id);

  void DrawXYZAxesOverlay();

  void DrawBoundingBox(const StaticMeshId sm_id, const Transform& transform);
  void DrawTerminal(const StaticMeshId sm_id, const MaterialID mat_id,
                    const Transform& transform);

  VertexArrayManager vertex_array_mgr;
  GLBufferManager gl_buffer_mgr;

  VertexArrayID vao_id = 0, text_vao = 0;
  GLBufferID text_vbo_id = 0;
  ShaderID shader_id, font_shader_id;
  GLTextureID texture_id;
  SamplerID sampler_id;
  SamplerID sampler_id2;

  ShaderManager shader_mgr;

 private:
  QuixotismRenderer();

  void CompileTextShader();

  std::vector<TextDrawInfo> draw_text_queue;

  std::unique_ptr<u8[]> cached_text_vert_buffer;
  size_t cached_text_vert_buffer_size = 0;

  GLTextureManager texture_mgr;
  GLSamplerManager sampler_mgr;
};

}  // namespace quixotism
