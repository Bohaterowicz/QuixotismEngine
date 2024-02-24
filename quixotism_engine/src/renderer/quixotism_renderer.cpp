#include "quixotism_renderer.hpp"

#include "GL/glew.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include "core/quixotism_engine.hpp"
#include "dbg_print.hpp"
#include "file_processing/obj_parser/obj_parser.hpp"
#include "gl_call.hpp"
#include "vertex_buffer_layout.hpp"

namespace quixotism {

QuixotismRenderer::QuixotismRenderer() {
  auto dim = QuixotismEngine::GetEngine().GetWindowDim();
  GLCall(glViewport(0, 0, dim.width, dim.height));

  GLCall(glEnable(GL_DEPTH_TEST));
  // GLCall(glEnable(GL_CULL_FACE));
  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  GLCall(glCullFace(GL_BACK));

  GLCall(glClearColor(1.0F, 0.0F, 0.0F, 1.0F));
  GLCall(glClearDepth(1.0F));
  GLCall(glClearStencil(0));
  DBG_PRINT("Specified clear colors");

  VertexBufferLayout layout;
  layout.AddLayoutElementF(3, false, 0);
  layout.AddLayoutElementF(2, false, 0);
  if (auto new_vao = vertex_array_mgr.Create(layout)) {
    vao_id = *new_vao;
  } else {
    assert(0);
  }

  ShaderStageSpec shader_spec;
  shader_spec.emplace_back(
      ShaderStageType::VERTEX,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/basic.vert");
  shader_spec.emplace_back(
      ShaderStageType::FRAGMENT,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/basic.frag");
  shader_id = shader_mgr.CreateShader(shader_spec);

  shader_spec = {};
  shader_spec.emplace_back(
      ShaderStageType::VERTEX,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/model.vert");
  shader_spec.emplace_back(
      ShaderStageType::FRAGMENT,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/model.frag");
  shader_id2 = shader_mgr.CreateShader(shader_spec);

  CompileTextShader();

  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  auto &bitmap = QuixotismEngine::GetEngine().font.GetBitmap();
  auto [width, height] = bitmap.GetDim();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED,
               GL_UNSIGNED_BYTE, bitmap.GetBitmapPtr());
  glGenerateTextureMipmap(texture1);
}

void QuixotismRenderer::ClearRenderTarget() {
  GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                 GL_STENCIL_BUFFER_BIT));
}

void QuixotismRenderer::CompileTextShader() {
  const char *vertex_source_path =
      "D:/QuixotismEngine/quixotism_engine/data/shaders/font_basic.vert";
  const char *fragment_source_path =
      "D:/QuixotismEngine/quixotism_engine/data/shaders/font_basic.frag";

  ShaderStageSpec shader_spec;
  shader_spec.emplace_back(ShaderStageType::VERTEX, vertex_source_path);
  shader_spec.emplace_back(ShaderStageType::FRAGMENT, fragment_source_path);
  font_shader_id = shader_mgr.CreateShader(shader_spec);
}

void QuixotismRenderer::PushText(std::string &&text, Vec2 position, r32 scale) {
  TextDrawInfo info;
  info.text = std::move(text);
  info.position = position;
  info.scale = scale;
  draw_text_queue.push_back(std::move(info));
}

struct GlyphVert {
  r32 pos[2];
  r32 coord[2];
};

void QuixotismRenderer::DrawText() {
  // first compute the buffer size needed to store vertex data or all characters
  // for all text to render
  size_t char_count = 0;
  for (const auto &text_info : draw_text_queue) {
    char_count += text_info.text.size();
  }

  // for now we hard code that we use 6 verts per character (quad = 2 triangles)
  // with 4 floats per vertex (x, y, u, v)
  static constexpr size_t vertex_size = 4 * sizeof(r32);
  static constexpr size_t verts_per_triangle = 3;
  size_t triangle_count = char_count * 2;
  size_t vertex_count = triangle_count * verts_per_triangle;
  size_t required_buffer_size = vertex_count * vertex_size;
  if (required_buffer_size == 0) {
    return;
  }

  if (cached_text_vert_buffer_size < required_buffer_size) {
    size_t buffer_size = 512;
    while (buffer_size < required_buffer_size) {
      buffer_size <<= 1;
    }
    cached_text_vert_buffer = std::make_unique<u8[]>(buffer_size);
    cached_text_vert_buffer_size = buffer_size;
  }

  assert(cached_text_vert_buffer &&
         cached_text_vert_buffer_size >= required_buffer_size);

  // now we fill the vertex data buffer
  auto &font = QuixotismEngine::GetEngine().font;
  auto space_advance = font.GetSpaceAdvance();
  auto *vert = reinterpret_cast<GlyphVert *>(cached_text_vert_buffer.get());
  auto window_dim = QuixotismEngine::GetEngine().GetWindowDim();
  for (const auto &text_info : draw_text_queue) {
    r32 position_x = text_info.position.x;
    u32 codepoint = 0, prev_codepoint = 0;
    r32 screen_scale_x = static_cast<r32>(window_dim.width) / text_info.scale;
    r32 screen_scale_y = static_cast<r32>(window_dim.height) / text_info.scale;
    for (const auto &c : text_info.text) {
      codepoint = c;
      if (codepoint == ' ') {
        position_x += space_advance / screen_scale_x;
        continue;
      }
      auto &coord = font.GetCodepointBitmapCoord(codepoint);

      auto &glyph_info = font.GetGlyphInfo(codepoint);
      auto glyph_width = static_cast<r32>(glyph_info.width) / screen_scale_x;
      auto glyph_height = static_cast<r32>(glyph_info.height) / screen_scale_y;
      auto baseline_offset =
          static_cast<r32>(glyph_info.height + glyph_info.baseline_offset);

      position_x =
          position_x + (font.GetHorizontalAdvance(codepoint, prev_codepoint) /
                        screen_scale_x);
      auto position_y =
          text_info.position.y - (baseline_offset / screen_scale_y);
      // tri1
      vert[0].pos[0] = position_x;
      vert[0].pos[1] = position_y;
      vert[0].coord[0] = coord.lower_left.x;
      vert[0].coord[1] = coord.lower_left.y;

      vert[1].pos[0] = position_x + glyph_width;
      vert[1].pos[1] = position_y;
      vert[1].coord[0] = coord.top_right.x;
      vert[1].coord[1] = coord.lower_left.y;

      vert[2].pos[0] = position_x;
      vert[2].pos[1] = position_y + glyph_height;
      vert[2].coord[0] = coord.lower_left.x;
      vert[2].coord[1] = coord.top_right.y;

      // tri2
      vert[3].pos[0] = position_x + glyph_width;
      vert[3].pos[1] = position_y;
      vert[3].coord[0] = coord.top_right.x;
      vert[3].coord[1] = coord.lower_left.y;

      vert[4].pos[0] = position_x + glyph_width;
      vert[4].pos[1] = position_y + glyph_height;
      vert[4].coord[0] = coord.top_right.x;
      vert[4].coord[1] = coord.top_right.y;

      vert[5].pos[0] = position_x;
      vert[5].pos[1] = position_y + glyph_height;
      vert[5].coord[0] = coord.lower_left.x;
      vert[5].coord[1] = coord.top_right.y;

      prev_codepoint = codepoint;
      vert += 6;  // go over the 6 verticies we just wrote
    }
  }

  draw_text_queue.clear();

  // potentially create buffer on gpu (if we did not have one already)
  if (!text_vbo_id) {
    if (auto id = gl_buffer_mgr.Create()) {
      text_vbo_id = id;
    } else {
      assert(0);
    }
  }

  // if no font vao, create one
  if (!text_vao) {
    VertexBufferLayout font_vao_layout;
    font_vao_layout.AddLayoutElementF(2, false, 0);
    font_vao_layout.AddLayoutElementF(2, false, 0);
    if (auto vao_id = vertex_array_mgr.Create(font_vao_layout)) {
      text_vao = *vao_id;
    } else {
      assert(0);
    }
  }

  auto vbo = gl_buffer_mgr.Get(text_vbo_id);
  if (!vbo) {
    assert(0);
  }
  // send vert data to gpu
  GLBufferData(*vbo, cached_text_vert_buffer.get(),
               cached_text_vert_buffer_size, BufferDataMode::STATIC_DRAW);

  // bind vert buffer
  auto vao = vertex_array_mgr.Get(text_vao);
  if (!vao) {
    assert(0);
  }

  BindVertexBufferToVertexArray(*vao, *vbo, 0);
  BindVertexArray(*vao);
  // setup shader
  auto font_shader = shader_mgr.Get(font_shader_id);
  assert(font_shader);
  GLCall(glUseProgram((*font_shader).id));
  GLCall(glUniform1i(glGetUniformLocation((*font_shader).id, "texture1"), 0));

  // draw call
  // disable depth testing for screen text rendering
  GLCall(glDepthMask(GL_FALSE));
  GLCall(glDrawArrays(GL_TRIANGLES, 0, vertex_count));
  // enable depth testing back again
  GLCall(glDepthMask(GL_TRUE));
}

void QuixotismRenderer::DrawStaticMeshes() {
  auto &engine = QuixotismEngine::GetEngine();

  auto shader = shader_mgr.Get(shader_id2);
  if (!shader) {
    assert(0);
  }
  glUseProgram((*shader).id);

  auto camera_id = engine.GetCamera();
  auto &camera = engine.entity_mgr.GetComponent<CameraComponent>(camera_id);
  auto &transform =
      engine.entity_mgr.GetComponent<TransformComponent>(camera_id);
  auto view = transform.GetOrientationMatrix();
  auto proj = camera.GetProjectionMatrix();

  int viewLoc = glGetUniformLocation((*shader).id, "view");
  GLCall(glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.DataPtr()));
  int projLoc = glGetUniformLocation((*shader).id, "projection");
  GLCall(glUniformMatrix4fv(projLoc, 1, GL_FALSE, proj.DataPtr()));

  for (auto &static_mesh : engine.static_mesh_mgr) {
    auto vao = vertex_array_mgr.Get(static_mesh.vao_id);
    auto vbo = gl_buffer_mgr.Get(static_mesh.vbo_id);
    auto ebo = gl_buffer_mgr.Get(static_mesh.ebo_id);
    if (!vbo || !ebo || !vao) {
      assert(0);
    }
    BindVertexArray(*vao);
    BindVertexBufferToVertexArray(*vao, *vbo, *ebo, 0);
    GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));
  }
}

void QuixotismRenderer::MakeDrawableStaticMesh(StaticMeshId id) {
  auto &engine = QuixotismEngine::GetEngine();
  auto sm_mesh = engine.static_mesh_mgr.Get(id);
  if (!sm_mesh) return;

  auto vbo_id = gl_buffer_mgr.Create();
  auto ebo_id = gl_buffer_mgr.Create();
  if (vbo_id && ebo_id) {
    sm_mesh->vbo_id = vbo_id;
    sm_mesh->ebo_id = ebo_id;
  } else {
    assert(0);
  }

  auto vao = *vertex_array_mgr.Get(vao_id);
  sm_mesh->vao_id = vao_id;

  auto &mesh = sm_mesh->GetMeshData();
  std::vector<void *> vertex_data_buffers = {mesh.VertexPosData.data(),
                                             mesh.VertexTexCoordData.data()};
  std::vector<u32 *> vertex_index_buffers = {
      mesh.VertexTriangleIndicies.PosIdx.data(),
      mesh.VertexTriangleIndicies.TexCoordIdx.data()};
  auto vertex_count = mesh.VertexTriangleIndicies.PosIdx.size();

  auto [vertex_buffer, vertex_buffer_size] = SerializeVertexDataByLayout(
      vertex_data_buffers, vertex_index_buffers, vertex_count, vao.layout);
  auto index_buffer = GenerateSerialIndexBuffer(vertex_count);
  size_t index_buffer_size = vertex_count * sizeof(u32);

  auto vbo_buf = gl_buffer_mgr.Get(sm_mesh->vbo_id);
  auto ebo_buf = gl_buffer_mgr.Get(sm_mesh->ebo_id);
  if (vbo_buf && ebo_buf) {
    GLBufferData(*vbo_buf, vertex_buffer.get(), vertex_buffer_size,
                 BufferDataMode::STATIC_DRAW);
    GLBufferData(*ebo_buf, index_buffer.get(), index_buffer_size,
                 BufferDataMode::STATIC_DRAW);
  } else {
    assert(0);
  }
}

}  // namespace quixotism
