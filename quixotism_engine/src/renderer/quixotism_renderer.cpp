#include "quixotism_renderer.hpp"

#include "GL/glew.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include "colors.hpp"
#include "core/quixotism_engine.hpp"
#include "dbg_print.hpp"
#include "file_processing/obj_parser/obj_parser.hpp"
#include "gl_call.hpp"
#include "vertex_buffer_layout.hpp"

namespace quixotism {

static ShaderID CreateBasicLineSader(ShaderManager &shader_mgr) {
  ShaderStageSpec shader_spec;
  shader_spec.emplace_back(
      ShaderStageType::VERTEX,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/basic_line.vert");
  shader_spec.emplace_back(
      ShaderStageType::FRAGMENT,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/basic_line.frag");
  return shader_mgr.CreateShader("basic_line", shader_spec);
}

static ShaderID CreateBasicXYZAxesSader(ShaderManager &shader_mgr) {
  ShaderStageSpec shader_spec;
  shader_spec.emplace_back(
      ShaderStageType::VERTEX,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/xyz_axes.vert");
  shader_spec.emplace_back(
      ShaderStageType::FRAGMENT,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/xyz_axes.frag");
  return shader_mgr.CreateShader("xyz_axes", shader_spec);
}

QuixotismRenderer::QuixotismRenderer() {
  auto dim = QuixotismEngine::GetEngine().GetWindowDim();
  GLCall(glViewport(0, 0, dim.width, dim.height));

  GLCall(glEnable(GL_DEPTH_TEST));
  GLCall(glEnable(GL_CULL_FACE));
  GLCall(glEnable(GL_BLEND));
  GLCall(glEnable(GL_LINE_SMOOTH));
  GLCall(glEnable(GL_STENCIL_TEST));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  GLCall(glCullFace(GL_BACK));

  GLCall(glClearColor(0.4F, 0.4F, 0.4F, 1.0F));
  GLCall(glClearDepth(1.0F));
  GLCall(glClearStencil(0));
  DBG_PRINT("Specified clear colors");

  VertexBufferLayout layout;
  layout.AddLayoutElementF(3, false, 0);
  layout.AddLayoutElementF(3, false, 0);
  layout.AddLayoutElementF(2, false, 0);
  if (auto new_vao = vertex_array_mgr.Create(layout)) {
    vao_id = *new_vao;
  } else {
    Assert(0);
  }

  ShaderStageSpec shader_spec;
  shader_spec.emplace_back(
      ShaderStageType::VERTEX,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/basic.vert");
  shader_spec.emplace_back(
      ShaderStageType::FRAGMENT,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/basic.frag");
  shader_id = shader_mgr.CreateShader("basic", shader_spec);

  shader_spec = {};
  shader_spec.emplace_back(
      ShaderStageType::VERTEX,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/model.vert");
  shader_spec.emplace_back(
      ShaderStageType::FRAGMENT,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/model.frag");
  shader_mgr.CreateShader("model", shader_spec);

  shader_spec = {};
  shader_spec.emplace_back(ShaderStageType::VERTEX,
                           "D:/QuixotismEngine/quixotism_engine/data/shaders/"
                           "basic_v3_model_only.vert");
  shader_spec.emplace_back(
      ShaderStageType::FRAGMENT,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/solid_color.frag");
  shader_mgr.CreateShader("solid_color", shader_spec);

  shader_spec = {};
  shader_spec.emplace_back(ShaderStageType::VERTEX,
                           "D:/QuixotismEngine/quixotism_engine/data/shaders/"
                           "skybox.vert");
  shader_spec.emplace_back(
      ShaderStageType::FRAGMENT,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/skybox.frag");
  shader_mgr.CreateShader("skybox", shader_spec);

  shader_spec = {};
  shader_spec.emplace_back(ShaderStageType::VERTEX,
                           "D:/QuixotismEngine/quixotism_engine/data/shaders/"
                           "screen_quad.vert");
  shader_spec.emplace_back(
      ShaderStageType::FRAGMENT,
      "D:/QuixotismEngine/quixotism_engine/data/shaders/screen_quad.frag");
  screen_quad_shader_id = shader_mgr.CreateShader("screen_quad", shader_spec);

  CompileTextShader();
  auto sampler = CreateSampler();
  sampler_id = sampler_mgr.Add(std::move(sampler));
  sampler_id2 = sampler_mgr.Add(CreateSampler2());
  cube_sampler = sampler_mgr.Add(CreateCubeSampler());

  for (auto &font_set : QuixotismEngine::GetEngine().font_mgr) {
    auto [max_width, max_height] = font_set.fonts[4].GetBitmap().GetDim();
    Bitmap bitmap_arr[4];
    const Bitmap *bitmap_ptr_arr[5] = {};
    bitmap_ptr_arr[4] = &font_set.fonts[4].GetBitmap();
    for (u32 idx = 0; idx < (ArrayCount(FontSet::font_sizes) - 1); ++idx) {
      auto &font = font_set.fonts[idx];
      auto &bitmap = font.GetBitmap();
      Bitmap tmp_bitmap{(u32)max_width, (u32)max_height, BitmapFormat::R8};
      CopyBitmap(bitmap, tmp_bitmap);
      bitmap_arr[idx] = std::move(tmp_bitmap);
      bitmap_ptr_arr[idx] = &bitmap_arr[idx];
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    auto font_texture = CreateTextureArray(&bitmap_ptr_arr[0], 5, true);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    font_set.texture_id = texture_mgr.Add(std::move(font_texture));
  }
}

void QuixotismRenderer::DrawOutline() {
  auto &engine = QuixotismEngine::GetEngine();
  auto dim = engine.GetWindowDim();
  static ShaderID jfa_init_shader_id = 0;
  static ShaderID jfa_shader_id = 0;
  static ShaderID outline_shader_id = 0;
  static u64 tex1 = 0;
  static u64 tex2 = 0;
  static Framebuffer fb;
  static Framebuffer fb2;
  if (!jfa_init_shader_id) {
    ShaderStageSpec shader_spec;
    shader_spec.emplace_back(ShaderStageType::VERTEX,
                             "D:/QuixotismEngine/quixotism_engine/data/shaders/"
                             "compute/jfa_init.vert");
    shader_spec.emplace_back(ShaderStageType::FRAGMENT,
                             "D:/QuixotismEngine/quixotism_engine/data/shaders/"
                             "compute/jfa_init.frag");
    jfa_init_shader_id = shader_mgr.CreateShader("jfa_init", shader_spec);
    shader_spec = {};
    shader_spec.emplace_back(ShaderStageType::COMPUTE,
                             "D:/QuixotismEngine/quixotism_engine/data/shaders/"
                             "compute/jfa.glsl");
    jfa_shader_id = shader_mgr.CreateShader("jfa", shader_spec);
    shader_spec = {};
    shader_spec.emplace_back(ShaderStageType::VERTEX,
                             "D:/QuixotismEngine/quixotism_engine/data/shaders/"
                             "compute/jfa_init.vert");
    shader_spec.emplace_back(ShaderStageType::FRAGMENT,
                             "D:/QuixotismEngine/quixotism_engine/data/shaders/"
                             "compute/jfa_outline.frag");
    outline_shader_id = shader_mgr.CreateShader("outline", shader_spec);

    FramebufferDesc fbo_desc;
    auto ds_id = offscreen_fbo.attachments[1]
                     .second;  // hardcoded for now because I am lazy...
    TextureDesc tex_desc;
    tex_desc.type = TextureType::TEXTURE_2D;
    tex_desc.format = BitmapFormat::RG32F;
    tex_desc.width = dim.width;
    tex_desc.height = dim.height;
    tex_desc.mip_levels = 1;
    tex1 = texture_mgr.CreateTexture(tex_desc);
    tex2 = texture_mgr.CreateTexture(tex_desc);
    fbo_desc.attachments.emplace_back(AttachmentType::COLOR0, tex1);
    fbo_desc.attachments.emplace_back(AttachmentType::DEPTH_STENCIL, ds_id);
    fb = std::move(CreateFramebuffer(fbo_desc));
  }

  fb.Bind();
  GLCall(glClearColor(-1.0F, -1.0F, -1.0F, 1.0F));
  GLCall(glClear(GL_COLOR_BUFFER_BIT));
  GLCall(glDisable(GL_DEPTH_TEST));
  GLCall(glEnable(GL_STENCIL_TEST));
  GLCall(glStencilFunc(GL_EQUAL, 1, 0xFF));
  GLCall(glStencilMask(0x00));
  auto jfa_init_shader = shader_mgr.Get(jfa_init_shader_id);
  GLCall(glUseProgram((*jfa_init_shader).id));
  BindVertexBufferToVertexArray(
      *vertex_array_mgr.Get(engine.screen_quad_mesh.vao_id),
      *gl_buffer_mgr.Get(engine.screen_quad_mesh.vbo_id), 0);
  BindVertexArray(*vertex_array_mgr.Get(engine.screen_quad_mesh.vao_id));
  GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

  auto jfa_shader = shader_mgr.Get(jfa_shader_id);
  auto read_tex = texture_mgr.Get(tex1)->Id();
  auto write_tex = texture_mgr.Get(tex2)->Id();
  GLCall(glUseProgram((*jfa_shader).id));
  i32 step_size = 8;
  while (step_size >= 1) {
    jfa_shader->SetUniform("step_size", step_size);
    GLCall(glBindImageTexture(0, read_tex, 0, GL_FALSE, 0, GL_READ_ONLY,
                              GL_RG32F));
    GLCall(glBindImageTexture(1, write_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY,
                              GL_RG32F));
    GLCall(glDispatchCompute(dim.width, dim.height, 1));
    GLCall(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
    auto tmp = read_tex;
    read_tex = write_tex;
    write_tex = tmp;
    step_size /= 2;
  }
  offscreen_fbo.Bind();
  GLCall(glStencilFunc(GL_EQUAL, 0, 0xFF));
  BindVertexBufferToVertexArray(
      *vertex_array_mgr.Get(engine.screen_quad_mesh.vao_id),
      *gl_buffer_mgr.Get(engine.screen_quad_mesh.vbo_id), 0);
  BindVertexArray(*vertex_array_mgr.Get(engine.screen_quad_mesh.vao_id));
  auto outline_shader = shader_mgr.Get(outline_shader_id);
  GLCall(glUseProgram((*outline_shader).id));
  GLCall(glBindTextureUnit(0, read_tex));
  outline_shader->SetUniform("jfa_texture", 0);
  GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
  GLCall(glClearColor(0.4F, 0.4F, 0.4F, 1.0F));
  GLCall(glStencilMask(0xFF));
  GLCall(glDisable(GL_STENCIL_TEST));
  GLCall(glEnable(GL_DEPTH_TEST));
}

void QuixotismRenderer::BindScreenFramebuffer() {
  GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void QuixotismRenderer::ClearFramebuffer() {
  glClearStencil(0);
  GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                 GL_STENCIL_BUFFER_BIT));
}

void QuixotismRenderer::InitOffscreenFramebuffer() {
  auto &engine = QuixotismEngine::GetEngine();
  static bool init = true;
  if (init) {
    auto dim = engine.GetWindowDim();
    FramebufferDesc fbo_desc;
    TextureDesc tex_desc;
    tex_desc.type = TextureType::TEXTURE_2D;
    tex_desc.format = BitmapFormat::RGBA8;
    tex_desc.width = dim.width;
    tex_desc.height = dim.height;
    tex_desc.mip_levels = 1;
    auto color_tex_id = texture_mgr.CreateTexture(tex_desc);
    tex_desc.format = BitmapFormat::DEPTH24_STENCIL8;
    auto depth_stencil_tex_id = texture_mgr.CreateTexture(tex_desc);
    fbo_desc.attachments.emplace_back(AttachmentType::COLOR0, color_tex_id);
    fbo_desc.attachments.emplace_back(AttachmentType::DEPTH_STENCIL,
                                      depth_stencil_tex_id);
    offscreen_fbo = CreateFramebuffer(fbo_desc);
    init = false;
  }
}

void QuixotismRenderer::DrawToScreenQuad(const StaticMesh &mesh) {
  auto &engine = QuixotismEngine::GetEngine();
  GLCall(glDisable(GL_DEPTH_TEST));
  auto sq_shader = shader_mgr.Get(screen_quad_shader_id);
  GLCall(glUseProgram((*sq_shader).id));
  BindVertexBufferToVertexArray(*vertex_array_mgr.Get(mesh.vao_id),
                                *gl_buffer_mgr.Get(mesh.vbo_id), 0);
  BindVertexArray(*vertex_array_mgr.Get(mesh.vao_id));
  auto color_id = offscreen_fbo.attachments[0]
                      .second;  // hardcoded for now because I am lazy...
  auto screen_tex = texture_mgr.Get(color_id);
  screen_tex->BindUnit(0);
  auto *sampler = sampler_mgr.Get(sampler_id2);
  sq_shader->SetUniform("screenTexture", 0);
  GLCall(glBindSampler(0, sampler->Id()));
  GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
  GLCall(glEnable(GL_DEPTH_TEST));
}

void QuixotismRenderer::CompileTextShader() {
  const char *vertex_source_path =
      "D:/QuixotismEngine/quixotism_engine/data/shaders/font_basic.vert";
  const char *fragment_source_path =
      "D:/QuixotismEngine/quixotism_engine/data/shaders/font_basic.frag";

  ShaderStageSpec shader_spec;
  shader_spec.emplace_back(ShaderStageType::VERTEX, vertex_source_path);
  shader_spec.emplace_back(ShaderStageType::FRAGMENT, fragment_source_path);
  font_shader_id = shader_mgr.CreateShader("font_basic", shader_spec);
}

void QuixotismRenderer::PushText(std::string &&text, Vec2 position, r32 scale,
                                 u32 layer, FontID font_id) {
  PushText(std::move(text), position, Color::WHITE, scale, layer, font_id);
}

void QuixotismRenderer::PushText(std::string &&text, Vec2 position, Vec3 color,
                                 r32 scale, u32 layer, FontID font_id) {
  TextDrawInfo info;
  info.text = std::move(text);
  info.position = position;
  info.color = color;
  info.scale = scale;
  info.layer = layer;
  if (!font_id) font_id = QuixotismEngine::GetEngine().font_mgr.GetDefault();
  info.font_id = font_id;
  draw_text_queue[font_id].push_back(std::move(info));
}

struct GlyphVert {
  r32 pos[2];
  r32 coord[3];
  r32 color[3];
};

void QuixotismRenderer::DrawText(u32 layer) {
  // for now we hard code that we use 6 verts per character (quad = 2
  // triangles) with 8 floats per vertex (x, y, u, v, z, r, g, b)
  static constexpr size_t vertex_size = 8 * sizeof(r32);
  static constexpr size_t verts_per_triangle = 3;
  for (const auto &[font_id, text_queue] : draw_text_queue) {
    // first compute the buffer size needed to store vertex data or all
    // characters
    // for all text to render
    size_t char_count = 0;
    auto count_characters_no_space = [](const std::string &str) {
      size_t count = 0;
      for (const auto &c : str) {
        if (c != ' ') ++count;
      }
      return count;
    };

    for (const auto &text_info : text_queue) {
      if (text_info.layer != layer) continue;
      char_count += count_characters_no_space(text_info.text);
    }

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

    Assert(cached_text_vert_buffer &&
           cached_text_vert_buffer_size >= required_buffer_size);

    // now we fill the vertex data buffer
    auto *vert = reinterpret_cast<GlyphVert *>(cached_text_vert_buffer.get());
    auto window_dim = QuixotismEngine::GetEngine().GetWindowDim();
    for (const auto &text_info : text_queue) {
      if (text_info.layer != layer) continue;
      auto *font = QuixotismEngine::GetEngine().font_mgr.GetByFontScale(
          text_info.font_id, text_info.scale);
      auto font_idx = QuixotismEngine::GetEngine().font_mgr.GetFontIdxByScale(
          text_info.font_id, text_info.scale);
      r32 scale_adjust = text_info.scale / font->GetScale();
      r32 position_x = text_info.position.x;
      u32 codepoint = 0, prev_codepoint = 0;
      r32 screen_scale_x = 2.0f / static_cast<r32>(window_dim.width);
      r32 screen_scale_y = 2.0f / static_cast<r32>(window_dim.height);

      // hardcoded for now becuase I am lazy...
      r32 uv_adjust = font->GetBitmap().GetHeight() / 512.0f;

      auto space_advance = font->GetSpaceAdvance() * text_info.scale;
      for (const auto &c : text_info.text) {
        codepoint = c;
        if (codepoint == ' ') {
          position_x += space_advance * screen_scale_x;
          continue;
        }
        auto &coord = font->GetCodepointBitmapCoord(codepoint);

        auto &glyph_info = font->GetGlyphInfo(codepoint);
        auto glyph_width = (static_cast<r32>(glyph_info.width) * scale_adjust) *
                           screen_scale_x;
        auto glyph_height =
            (static_cast<r32>(glyph_info.height) * scale_adjust) *
            screen_scale_y;
        auto baseline_offset =
            (static_cast<r32>(glyph_info.height + glyph_info.baseline_offset) *
             scale_adjust);

        position_x = position_x +
                     ((font->GetHorizontalAdvance(codepoint, prev_codepoint) *
                       scale_adjust) *
                      screen_scale_x);
        auto position_y =
            text_info.position.y - (baseline_offset * screen_scale_y);
        // tri1
        vert[0].pos[0] = position_x;
        vert[0].pos[1] = position_y;
        vert[0].coord[0] = coord.lower_left.x * uv_adjust;
        vert[0].coord[1] = coord.lower_left.y * uv_adjust;
        vert[0].coord[2] = font_idx;
        vert[0].color[0] = text_info.color[0];
        vert[0].color[1] = text_info.color[1];
        vert[0].color[2] = text_info.color[2];

        vert[1].pos[0] = position_x + glyph_width;
        vert[1].pos[1] = position_y;
        vert[1].coord[0] = coord.top_right.x * uv_adjust;
        vert[1].coord[1] = coord.lower_left.y * uv_adjust;
        vert[1].coord[2] = font_idx;
        vert[1].color[0] = text_info.color[0];
        vert[1].color[1] = text_info.color[1];
        vert[1].color[2] = text_info.color[2];

        vert[2].pos[0] = position_x;
        vert[2].pos[1] = position_y + glyph_height;
        vert[2].coord[0] = coord.lower_left.x * uv_adjust;
        vert[2].coord[1] = coord.top_right.y * uv_adjust;
        vert[2].coord[2] = font_idx;
        vert[2].color[0] = text_info.color[0];
        vert[2].color[1] = text_info.color[1];
        vert[2].color[2] = text_info.color[2];

        // tri2
        vert[3].pos[0] = position_x + glyph_width;
        vert[3].pos[1] = position_y;
        vert[3].coord[0] = coord.top_right.x * uv_adjust;
        vert[3].coord[1] = coord.lower_left.y * uv_adjust;
        vert[3].coord[2] = font_idx;
        vert[3].color[0] = text_info.color[0];
        vert[3].color[1] = text_info.color[1];
        vert[3].color[2] = text_info.color[2];

        vert[4].pos[0] = position_x + glyph_width;
        vert[4].pos[1] = position_y + glyph_height;
        vert[4].coord[0] = coord.top_right.x * uv_adjust;
        vert[4].coord[1] = coord.top_right.y * uv_adjust;
        vert[4].coord[2] = font_idx;
        vert[4].color[0] = text_info.color[0];
        vert[4].color[1] = text_info.color[1];
        vert[4].color[2] = text_info.color[2];

        vert[5].pos[0] = position_x;
        vert[5].pos[1] = position_y + glyph_height;
        vert[5].coord[0] = coord.lower_left.x * uv_adjust;
        vert[5].coord[1] = coord.top_right.y * uv_adjust;
        vert[5].coord[2] = font_idx;
        vert[5].color[0] = text_info.color[0];
        vert[5].color[1] = text_info.color[1];
        vert[5].color[2] = text_info.color[2];

        prev_codepoint = codepoint;
        vert += 6;  // go over the 6 verticies we just wrote
      }
    }

    // potentially create buffer on gpu (if we did not have one already)
    if (!text_vbo_id) {
      if (auto id = gl_buffer_mgr.Create()) {
        text_vbo_id = id;
      } else {
        Assert(0);
      }
    }

    // if no font vao, create one
    if (!text_vao) {
      VertexBufferLayout font_vao_layout;
      font_vao_layout.AddLayoutElementF(2, false, 0);
      font_vao_layout.AddLayoutElementF(3, false, 0);
      font_vao_layout.AddLayoutElementF(3, false, 0);
      if (auto vao_id = vertex_array_mgr.Create(font_vao_layout)) {
        text_vao = *vao_id;
      } else {
        Assert(0);
      }
    }

    auto vbo = gl_buffer_mgr.Get(text_vbo_id);
    if (!vbo) {
      Assert(0);
    }
    // send vert data to gpu
    GLBufferData(*vbo, cached_text_vert_buffer.get(),
                 cached_text_vert_buffer_size, BufferDataMode::STATIC_DRAW);

    // bind vert buffer
    auto vao = vertex_array_mgr.Get(text_vao);
    if (!vao) {
      Assert(0);
    }

    BindVertexBufferToVertexArray(*vao, *vbo, 0);
    BindVertexArray(*vao);
    // setup shader
    auto *font_shader = shader_mgr.Get(font_shader_id);
    Assert(font_shader);
    GLCall(glUseProgram((*font_shader).id));
    auto *font_texture = texture_mgr.Get(
        QuixotismEngine::GetEngine().font_mgr.Get(font_id)->texture_id);
    font_texture->BindUnit(0);
    auto *sampler = sampler_mgr.Get(sampler_id);
    font_shader->SetUniform("tex_sampler", 0);
    GLCall(glBindSampler(0, sampler->Id()));

    // draw call
    // disable depth testing for screen text rendering
    GLCall(glDepthMask(GL_FALSE));
    GLCall(glDrawArrays(GL_TRIANGLES, 0, vertex_count));
    // enable depth testing back again
    GLCall(glDepthMask(GL_TRUE));
  }
}

void QuixotismRenderer::PrepareDrawStaticMeshes() {
  auto &engine = QuixotismEngine::GetEngine();

  auto shader = shader_mgr.Get(shader_mgr.GetByName("model"));
  if (!shader) {
    Assert(0);
  }
  GLCall(glUseProgram((*shader).id));

  auto camera_id = engine.GetCamera();
  auto *camera = engine.entity_mgr.GetComponent<CameraComponent>(camera_id);
  auto &transform = engine.entity_mgr.Get(camera_id)->transform;
  auto view = transform.GetTransformMatrix();
  auto proj = camera->GetProjectionMatrix();
  auto light_pos = Vec3{100, 100, 0};

  shader->SetUniform("view", view);
  shader->SetUniform("projection", proj);
  shader->SetUniform("view_pos", transform.position);
  shader->SetUniform("light_pos", light_pos);
}

void QuixotismRenderer::DrawTerminal(const StaticMeshId sm_id,
                                     const MaterialID material,
                                     const Transform &transform) {
  auto &engine = QuixotismEngine::GetEngine();
  auto *sm = engine.static_mesh_mgr.Get(sm_id);
  auto vao = vertex_array_mgr.Get(sm->vao_id);
  auto vbo = gl_buffer_mgr.Get(sm->vbo_id);
  if (!vao || !vbo) {
    Assert(0);
  }

  BindVertexBufferToVertexArray(*vao, *vbo, 0);
  BindVertexArray(*vao);
  auto mat = engine.material_mgr.Get(material);
  auto *shader = shader_mgr.Get(mat->GetShaderId());
  Assert(shader);
  GLCall(glUseProgram((*shader).id));
  shader->SetUniform("model", transform.GetOffsetMatrix());
  shader->SetUniform("Color", Vec4{0.2, 0.2, 0.2, 0.95});
  GLCall(glDepthMask(GL_FALSE));
  GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
  GLCall(glDepthMask(GL_TRUE));
}

void QuixotismRenderer::DrawAABB(const StaticMeshId sm_id,
                                 const Transform &transform) {
  static VertexArrayID bb_vao_id = VertexArray::INVALID_VAO_ID;
  static ShaderID bb_shader_id = Shader::INVALID_SHADER_ID;
  auto &engine = QuixotismEngine::GetEngine();
  auto *sm = engine.static_mesh_mgr.Get(sm_id);
  if (!bb_vao_id) {
    VertexBufferLayout vao_layout;
    vao_layout.AddLayoutElementF(3, false, 0);
    if (auto vao_id = vertex_array_mgr.Create(vao_layout)) {
      bb_vao_id = *vao_id;
    } else {
      Assert(0);
    }

    bb_shader_id = shader_mgr.GetByName("basic_line");
    if (!bb_shader_id) {
      bb_shader_id = CreateBasicLineSader(shader_mgr);
      Assert(bb_shader_id);
    }
  }

  auto vao = vertex_array_mgr.Get(bb_vao_id);
  auto vbo = gl_buffer_mgr.Get(sm->bb_vbo_id);
  if (!vao || !vbo) {
    Assert(0);
  }

  BindVertexBufferToVertexArray(*vao, *vbo, 0);
  BindVertexArray(*vao);

  auto camera_id = engine.GetCamera();
  auto *camera = engine.entity_mgr.Get(engine.GetCamera());

  auto *shader = shader_mgr.Get(bb_shader_id);
  Assert(shader);
  GLCall(glUseProgram((*shader).id));
  shader->SetUniform("model", transform.GetTransformMatrix());
  shader->SetUniform("view", camera->transform.GetTransformMatrix());
  shader->SetUniform(
      "projection",
      camera->GetComponent<CameraComponent>()->GetProjectionMatrix());

  GLCall(glLineWidth(1.5));
  shader->SetUniform("Color", Vec3{1, 0, 0});
  GLCall(glDrawArrays(GL_LINE_STRIP, 0, 18));
}

void QuixotismRenderer::DrawStaticMesh(const StaticMeshId sm_id,
                                       const MaterialID mat_id,
                                       const Transform &transform,
                                       bool selected) {
  auto &engine = QuixotismEngine::GetEngine();
  auto *sm = engine.static_mesh_mgr.Get(sm_id);
  auto *mat = engine.material_mgr.Get(mat_id);
  Assert(sm && mat);

  auto shader = shader_mgr.Get(mat->GetShaderId());
  if (!shader) {
    Assert(0);
  }

  auto diffuse_tex = engine.texture_mgr.Get(mat->diffuse);
  auto specular_tex = engine.texture_mgr.Get(mat->specular);
  diffuse_tex->glid.BindUnit(0);
  specular_tex->glid.BindUnit(1);
  auto *sampler = sampler_mgr.Get(sampler_id2);
  shader->SetUniform("diffuse_tex", 0);
  shader->SetUniform("specular_tex", 1);
  GLCall(glBindSampler(0, sampler->Id()));

  shader->SetUniform("model", transform.GetTransformMatrix());

  auto vao = vertex_array_mgr.Get(sm->vao_id);
  auto vbo = gl_buffer_mgr.Get(sm->vbo_id);
  auto ebo = gl_buffer_mgr.Get(sm->ebo_id);
  if (!vbo || !ebo || !vao) {
    Assert(0);
  }
  BindVertexArray(*vao);
  BindVertexBufferToVertexArray(*vao, *vbo, *ebo, 0);

  if (selected) {
    GLCall(glEnable(GL_STENCIL_TEST));
    GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
    GLCall(glStencilFunc(GL_ALWAYS, 1, 0xFF));
  }
  GLCall(glDrawElements(GL_TRIANGLES,
                        sm->GetMeshData().VertexTriangleIndicies.PosIdx.size(),
                        GL_UNSIGNED_INT, 0));
  GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP));
}

void QuixotismRenderer::DrawSkybox() {
  auto &engine = QuixotismEngine::GetEngine();
  auto sid = shader_mgr.GetByName("skybox");
  Assert(sid);
  auto shader = shader_mgr.Get(sid);
  if (!shader) {
    Assert(0);
  }
  GLCall(glUseProgram((*shader).id));
  auto *camera = engine.entity_mgr.Get(engine.GetCamera());
  shader->SetUniform(
      "projection",
      camera->GetComponent<CameraComponent>()->GetProjectionMatrix());
  shader->SetUniform("view", camera->transform.GetRotationMatrix());

  //

  static float skybox_verts[] = {
      // positions
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

  static GLBufferID skybox_vbo_id = GLBuffer::INVALID_BUFFER_ID;
  static VertexArrayID skybox_vao_id = VertexArray::INVALID_VAO_ID;
  static ShaderID skybox_shader_id = Shader::INVALID_SHADER_ID;

  if (!skybox_vbo_id) {
    if (auto id = gl_buffer_mgr.Create()) {
      skybox_vbo_id = id;
    } else {
      Assert(0);
    }
    auto vbo = gl_buffer_mgr.Get(skybox_vbo_id);
    GLBufferData(*vbo, skybox_verts, sizeof(skybox_verts),
                 BufferDataMode::STATIC_DRAW);
    VertexBufferLayout vao_layout;
    vao_layout.AddLayoutElementF(3, false, 0);
    if (auto vao_id = vertex_array_mgr.Create(vao_layout)) {
      skybox_vao_id = *vao_id;
    } else {
      Assert(0);
    }
  }

  Assert(skybox_vao_id && skybox_vbo_id);

  auto vao = vertex_array_mgr.Get(skybox_vao_id);
  auto vbo = gl_buffer_mgr.Get(skybox_vbo_id);
  if (!vao || !vbo) {
    Assert(0);
  }

  //

  auto skybox_tex = engine.texture_mgr.Get(engine.ctex_id);
  skybox_tex->glid.BindUnit(0);
  auto *sampler = sampler_mgr.Get(cube_sampler);
  shader->SetUniform("skybox", 0);
  GLCall(glBindSampler(0, sampler->Id()));

  BindVertexArray(*vao);
  BindVertexBufferToVertexArray(*vao, *vbo, 0);

  // change depth function so depth test passes when values
  // are equal to depth buffer's content
  GLCall(glDepthFunc(GL_LEQUAL));
  GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
  GLCall(glDepthFunc(GL_LESS));  // set depth function back to default
}

static inline std::pair<std::unique_ptr<u8[]>, size_t> CreateAABBVertexBuffer(
    const AABB &bb) {
  size_t size =
      18 * 3 * sizeof(r32);  // bounding box has 8 verts, each has 3 r32s
  auto buffer = std::make_unique<u8[]>(size);
  auto dst = (r32 *)buffer.get();

  // min vert
  *dst++ = bb.min.x;
  *dst++ = bb.min.y;
  *dst++ = bb.min.z;
  //
  *dst++ = bb.max.x;
  *dst++ = bb.min.y;
  *dst++ = bb.min.z;
  //
  *dst++ = bb.max.x;
  *dst++ = bb.max.y;
  *dst++ = bb.min.z;
  //
  *dst++ = bb.min.x;
  *dst++ = bb.max.y;
  *dst++ = bb.min.z;
  //
  *dst++ = bb.min.x;
  *dst++ = bb.min.y;
  *dst++ = bb.min.z;
  //
  *dst++ = bb.min.x;
  *dst++ = bb.min.y;
  *dst++ = bb.max.z;
  //
  *dst++ = bb.min.x;
  *dst++ = bb.max.y;
  *dst++ = bb.max.z;
  //
  *dst++ = bb.min.x;
  *dst++ = bb.max.y;
  *dst++ = bb.min.z;
  //
  *dst++ = bb.min.x;
  *dst++ = bb.min.y;
  *dst++ = bb.min.z;
  //
  *dst++ = bb.max.x;
  *dst++ = bb.min.y;
  *dst++ = bb.min.z;
  //
  *dst++ = bb.max.x;
  *dst++ = bb.min.y;
  *dst++ = bb.max.z;
  //
  *dst++ = bb.max.x;
  *dst++ = bb.max.y;
  *dst++ = bb.max.z;
  //
  *dst++ = bb.max.x;
  *dst++ = bb.max.y;
  *dst++ = bb.min.z;
  //
  *dst++ = bb.max.x;
  *dst++ = bb.min.y;
  *dst++ = bb.min.z;
  //
  *dst++ = bb.max.x;
  *dst++ = bb.min.y;
  *dst++ = bb.max.z;
  //
  *dst++ = bb.min.x;
  *dst++ = bb.min.y;
  *dst++ = bb.max.z;
  //
  *dst++ = bb.min.x;
  *dst++ = bb.max.y;
  *dst++ = bb.max.z;
  //
  *dst++ = bb.max.x;
  *dst++ = bb.max.y;
  *dst++ = bb.max.z;

  return {std::move(buffer), size};
}

// THIS IS STUPID...
void QuixotismRenderer::MakeDrawableStaticMesh2(StaticMeshId id,
                                                VertexArrayID vao_id) {
  auto &engine = QuixotismEngine::GetEngine();
  auto sm_mesh = engine.static_mesh_mgr.Get(id);
  if (!sm_mesh) return;

  auto vbo_id = gl_buffer_mgr.Create();
  if (vbo_id) {
    sm_mesh->vbo_id = vbo_id;
  } else {
    Assert(0);
  }

  auto vao = *vertex_array_mgr.Get(vao_id);
  sm_mesh->vao_id = vao_id;

  auto &mesh = sm_mesh->GetMeshData();
  std::vector<void *> vertex_data_buffers = {mesh.VertexPosData.data()};

  auto vbo_buf = gl_buffer_mgr.Get(sm_mesh->vbo_id);

  if (vbo_buf) {
    GLBufferData(*vbo_buf, mesh.VertexPosData.data(),
                 mesh.VertexPosData.size() * 3 * sizeof(r32),
                 BufferDataMode::STATIC_DRAW);
  } else {
    Assert(0);
  }
}

void QuixotismRenderer::MakeDrawableStaticMesh(StaticMeshId id) {
  auto &engine = QuixotismEngine::GetEngine();
  auto sm_mesh = engine.static_mesh_mgr.Get(id);
  if (!sm_mesh) return;

  auto vbo_id = gl_buffer_mgr.Create();
  auto bb_vbo_id = gl_buffer_mgr.Create();
  auto ebo_id = gl_buffer_mgr.Create();
  if (vbo_id && ebo_id && bb_vbo_id) {
    sm_mesh->vbo_id = vbo_id;
    sm_mesh->ebo_id = ebo_id;
    sm_mesh->bb_vbo_id = bb_vbo_id;
  } else {
    Assert(0);
  }

  auto vao = *vertex_array_mgr.Get(vao_id);
  sm_mesh->vao_id = vao_id;

  auto &mesh = sm_mesh->GetMeshData();
  std::vector<void *> vertex_data_buffers = {mesh.VertexPosData.data(),
                                             mesh.VertexNormalData.data(),
                                             mesh.VertexTexCoordData.data()};
  std::vector<u32 *> vertex_index_buffers = {
      mesh.VertexTriangleIndicies.PosIdx.data(),
      mesh.VertexTriangleIndicies.NormalIdx.data(),
      mesh.VertexTriangleIndicies.TexCoordIdx.data()};
  auto vertex_count = mesh.VertexTriangleIndicies.PosIdx.size();

  auto [vertex_buffer, vertex_buffer_size] = SerializeVertexDataByLayout(
      vertex_data_buffers, vertex_index_buffers, vertex_count, vao.layout);
  auto index_buffer = GenerateSerialIndexBuffer(vertex_count);
  size_t index_buffer_size = vertex_count * sizeof(u32);

  auto [bb_buffer, bb_buffer_size] = CreateAABBVertexBuffer(mesh.bbox);

  auto vbo_buf = gl_buffer_mgr.Get(sm_mesh->vbo_id);
  auto ebo_buf = gl_buffer_mgr.Get(sm_mesh->ebo_id);
  auto bb_vbo_buf = gl_buffer_mgr.Get(sm_mesh->bb_vbo_id);
  if (vbo_buf && ebo_buf && bb_vbo_buf) {
    GLBufferData(*vbo_buf, vertex_buffer.get(), vertex_buffer_size,
                 BufferDataMode::STATIC_DRAW);
    GLBufferData(*ebo_buf, index_buffer.get(), index_buffer_size,
                 BufferDataMode::STATIC_DRAW);
    GLBufferData(*bb_vbo_buf, bb_buffer.get(), bb_buffer_size,
                 BufferDataMode::STATIC_DRAW);
  } else {
    Assert(0);
  }
}

void QuixotismRenderer::DrawXYZAxesOverlay() {
  static GLBufferID axes_vbo_id = GLBuffer::INVALID_BUFFER_ID;
  static VertexArrayID axes_vao_id = VertexArray::INVALID_VAO_ID;
  static ShaderID axes_shader_id = Shader::INVALID_SHADER_ID;
  static std::array<Vec3, 12> axes_verts{
      Vec3{0, 0, 0}, Vec3{1, 0, 0}, Vec3{1, 0, 0}, Vec3{1, 0, 0},
      Vec3{0, 0, 0}, Vec3{0, 1, 0}, Vec3{0, 1, 0}, Vec3{0, 1, 0},
      Vec3{0, 0, 0}, Vec3{0, 0, 1}, Vec3{0, 0, 1}, Vec3{0, 0, 1}};

  if (!axes_vbo_id) {
    axes_shader_id = shader_mgr.GetByName("xyz_axes");
    if (!axes_shader_id) {
      axes_shader_id = CreateBasicXYZAxesSader(shader_mgr);
      Assert(axes_shader_id);
    }

    if (auto id = gl_buffer_mgr.Create()) {
      axes_vbo_id = id;
    } else {
      Assert(0);
    }
    auto vbo = gl_buffer_mgr.Get(axes_vbo_id);
    GLBufferData(*vbo, axes_verts.data(), sizeof(axes_verts),
                 BufferDataMode::STATIC_DRAW);
    VertexBufferLayout vao_layout;
    vao_layout.AddLayoutElementF(3, false, 0);
    vao_layout.AddLayoutElementF(3, false, 0);
    if (auto vao_id = vertex_array_mgr.Create(vao_layout)) {
      axes_vao_id = *vao_id;
    } else {
      Assert(0);
    }
  }

  Assert(axes_vao_id && axes_vbo_id && axes_shader_id);

  auto &engine = QuixotismEngine::GetEngine();
  auto window_dim = engine.GetWindowDim();

  auto vao = vertex_array_mgr.Get(axes_vao_id);
  auto vbo = gl_buffer_mgr.Get(axes_vbo_id);
  if (!vao || !vbo) {
    Assert(0);
  }

  BindVertexBufferToVertexArray(*vao, *vbo, 0);
  BindVertexArray(*vao);

  auto camera_id = engine.GetCamera();
  auto *camera = engine.entity_mgr.GetComponent<CameraComponent>(camera_id);

  auto *shader = shader_mgr.Get(axes_shader_id);
  Assert(shader);
  GLCall(glUseProgram((*shader).id));
  auto &transform = engine.entity_mgr.Get(engine.GetCamera())->transform;
  shader->SetUniform("view", transform.GetRotationMatrix());
  shader->SetUniform("projection", camera->GetProjectionMatrix());

  GLCall(glLineWidth(3.0));
  GLCall(glViewport(0, 0, 100, 100));
  GLCall(glDrawArrays(GL_LINES, 0, 6));
  auto dim = QuixotismEngine::GetEngine().GetWindowDim();
  GLCall(glViewport(0, 0, dim.width, dim.height));
}

void QuixotismRenderer::CreateScreenQuad(StaticMesh &mesh) {
  r32 quadVertices[] = {-1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                        0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

                        -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
                        1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};

  auto vbo_id = gl_buffer_mgr.Create();
  if (vbo_id) {
    mesh.vbo_id = vbo_id;
  } else {
    Assert(0);
  }

  VertexBufferLayout vao_layout;
  vao_layout.AddLayoutElementF(2, false, 0);
  vao_layout.AddLayoutElementF(2, false, 0);
  if (auto vao_id = vertex_array_mgr.Create(vao_layout)) {
    mesh.vao_id = *vao_id;
  } else {
    Assert(0);
  }

  auto vbo = gl_buffer_mgr.Get(vbo_id);
  GLBufferData(*vbo, quadVertices, sizeof(quadVertices),
               BufferDataMode::STATIC_DRAW);
}

}  // namespace quixotism
