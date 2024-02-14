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

void QuixotismRenderer::Test() {
  static bool init = false;
  if (!init) {
    init = true;

    CompileTextShader();

    auto obj_data = QuixotismEngine::GetEngine().services.read_file(
        "D:/QuixotismEngine/quixotism_engine/data/meshes/box.obj");
    auto meshes = ParseOBJ(obj_data.data.get(), obj_data.size);

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

    float vertices[] = {
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  // top right
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // bottom left
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f   // top left
    };

    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    auto vbo_id = gl_buffer_mgr.Create();
    auto ebo_id = gl_buffer_mgr.Create();
    if (vbo_id && ebo_id) {
      vbo = vbo_id;
      ebo = ebo_id;
    } else {
      assert(0);
    }

    vbo_id = gl_buffer_mgr.Create();
    ebo_id = gl_buffer_mgr.Create();
    if (vbo_id && ebo_id) {
      vbo2 = vbo_id;
      ebo2 = ebo_id;
    } else {
      assert(0);
    }

    VertexBufferLayout layout;
    layout.AddLayoutElementF(3, false, 0);
    layout.AddLayoutElementF(2, false, 0);
    if (auto vao_id = vertex_array_mgr.Create(layout)) {
      vao = *vao_id;
    } else {
      assert(0);
    }

    auto vbo_buf = gl_buffer_mgr.Get(vbo);
    auto ebo_buf = gl_buffer_mgr.Get(ebo);
    if (vbo_buf && ebo_buf) {
      GLBufferData(*vbo_buf, vertices, sizeof(vertices),
                   BufferDataMode::STATIC_DRAW);
      GLBufferData(*ebo_buf, indices, sizeof(indices),
                   BufferDataMode::STATIC_DRAW);
    }

    std::vector<void *> vertex_data_buffers = {
        meshes[0].VertexPosData.data(), meshes[0].VertexTexCoordData.data()};
    std::vector<u32 *> vertex_index_buffers = {
        meshes[0].VertexTriangleIndicies.PosIdx.data(),
        meshes[0].VertexTriangleIndicies.TexCoordIdx.data()};
    auto vertex_count = meshes[0].VertexTriangleIndicies.PosIdx.size();
    auto [vertex_buffer, vertex_buffer_size] = SerializeVertexDataByLayout(
        vertex_data_buffers, vertex_index_buffers, vertex_count, layout);
    auto index_buffer = GenerateSerialIndexBuffer(vertex_count);
    size_t index_buffer_size = vertex_count * sizeof(u32);

    vbo_buf = gl_buffer_mgr.Get(vbo2);
    ebo_buf = gl_buffer_mgr.Get(ebo2);
    if (vbo_buf && ebo_buf) {
      GLBufferData(*vbo_buf, vertex_buffer.get(), vertex_buffer_size,
                   BufferDataMode::STATIC_DRAW);
      GLBufferData(*ebo_buf, index_buffer.get(), index_buffer_size,
                   BufferDataMode::STATIC_DRAW);
    }

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
  auto shader = shader_mgr.Get(shader_id);
  if (!shader) {
    assert(0);
  }
  glUseProgram((*shader).id);
  glUniform1i(glGetUniformLocation((*shader).id, "texture1"), 0);
  auto vbo_buf = gl_buffer_mgr.Get(vbo);
  auto ebo_buf = gl_buffer_mgr.Get(ebo);
  auto vao_ = vertex_array_mgr.Get(vao);
  if (!vbo_buf || !ebo_buf || !vao_) {
    assert(0);
  }
  BindVertexArray(*vao_);
  BindVertexBufferToVertexArray(*vao_, *vbo_buf, *ebo_buf, 0);
  // GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

  shader = shader_mgr.Get(shader_id2);
  if (!shader) {
    assert(0);
  }
  glUseProgram((*shader).id);

  auto camera_id = QuixotismEngine::GetEngine().GetCamera();
  auto &camera =
      QuixotismEngine::GetEngine().entity_mgr.GetComponent<CameraComponent>(
          camera_id);
  auto &transform =
      QuixotismEngine::GetEngine().entity_mgr.GetComponent<TransformComponent>(
          camera_id);
  auto view = transform.GetOrientationMatrix();
  auto proj = camera.GetProjectionMatrix();

  int viewLoc = glGetUniformLocation((*shader).id, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.DataPtr());
  int projLoc = glGetUniformLocation((*shader).id, "projection");
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, proj.DataPtr());

  vbo_buf = gl_buffer_mgr.Get(vbo2);
  ebo_buf = gl_buffer_mgr.Get(ebo2);
  if (!vbo_buf || !ebo_buf) {
    assert(0);
  }
  BindVertexBufferToVertexArray(*vao_, *vbo_buf, *ebo_buf, 0);
  GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));
  DrawText();
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
      auto coord = font.GetCodepointBitmapCoord(codepoint);

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

}  // namespace quixotism
