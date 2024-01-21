#include "quixotism_renderer.hpp"

#include "GL/glew.h"
#include "core/quixotism_engine.hpp"
#include "dbg_print.hpp"
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
  const char *vertexShaderSource =
      "#version 330 core\n"
      "layout (location = 0) in vec2 aPos;\n"
      "layout (location = 1) in vec2 aTexCoord;\n"
      "out vec2 TexCoord;\n"
      "void main()\n"
      "{\n"
      "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
      "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
      "}\0";
  const char *fragmentShaderSource =
      "#version 330 core\n"
      "out vec4 FragColor;\n"
      "in vec2 TexCoord;\n"
      "uniform sampler2D texture1;\n"
      "void main()\n"
      "{\n"
      "   FragColor = vec4(1.0f, 1.0f, 1.0f, texture(texture1, TexCoord));\n"
      "}\n\0";
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  GLCall(glShaderSource(vertexShader, 1, &vertexShaderSource, NULL));
  GLCall(glCompileShader(vertexShader));

  int ErrorResult = 0;
  GLCall(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &ErrorResult));

  if (ErrorResult == GL_FALSE) {
    assert(0);
  }

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  GLCall(glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL));
  GLCall(glCompileShader(fragmentShader));

  GLCall(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &ErrorResult));

  if (ErrorResult == GL_FALSE) {
    assert(0);
  }

  text_shader_program = glCreateProgram();
  GLCall(glAttachShader(text_shader_program, vertexShader));
  GLCall(glAttachShader(text_shader_program, fragmentShader));
  GLCall(glLinkProgram(text_shader_program));
  GLCall(glValidateProgram(text_shader_program));

  GLCall(glDeleteShader(vertexShader));
  GLCall(glDeleteShader(fragmentShader));
}

void QuixotismRenderer::Test() {
  static bool init = false;
  if (!init) {
    init = true;

    CompileTextShader();

    const char *vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
        "}\0";
    const char *fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D texture1;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 1.0f, 1.0f, texture(texture1, TexCoord));\n"
        "}\n\0";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

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

    VertexBufferLayout layout;
    layout.AddLayoutElementF(3, false, 0);
    layout.AddLayoutElementF(2, false, 0);
    if (auto vao_id = vertex_array_mgr.Create(layout)) {
      vao = *vao_id;
    } else {
      assert(0);
    }

    VertexBufferLayout layout2;
    layout2.AddLayoutElementF(2, false, 0);
    layout2.AddLayoutElementF(2, false, 0);
    if (auto vao_id = vertex_array_mgr.Create(layout2)) {
      text_vao = *vao_id;
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

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    auto &bitmap = QuixotismEngine::GetEngine().font.GetBitmap();
    auto [width, height] = bitmap.GetDim();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED,
                 GL_UNSIGNED_BYTE, bitmap.GetBitmapPtr());
  }
  glUseProgram(shaderProgram);
  glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
  auto vbo_buf = gl_buffer_mgr.Get(vbo);
  auto ebo_buf = gl_buffer_mgr.Get(ebo);
  auto vao_ = vertex_array_mgr.Get(vao);
  if (!vbo_buf || !ebo_buf || !vao_) {
    assert(0);
  }
  BindVertexBufferToVertexArray(*vao_, *vbo_buf, *ebo_buf, 0);
  BindVertexArray(*vao_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  DrawText();
}

void QuixotismRenderer::PushText(std::string &&text, Vec2 position) {
  TextDrawInfo info;
  info.text = std::move(text);
  info.position = position;
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
  auto *vert = reinterpret_cast<GlyphVert *>(cached_text_vert_buffer.get());
  for (const auto &text_info : draw_text_queue) {
    r32 x_offset = 0;
    for (const auto &c : text_info.text) {
      if (c == ' ') {
        x_offset += 0.015;
        continue;
      }

      auto &glyph_info = font.GetGlyphInfo(c);
      auto glyph_width = 0.03;
      auto glyph_height = 0.05;
      // tri1
      vert[0].pos[0] = text_info.position.x + x_offset;
      vert[0].pos[1] = text_info.position.y;
      vert[0].coord[0] = glyph_info.coord.lower_left.x;
      vert[0].coord[1] = glyph_info.coord.lower_left.y;

      vert[1].pos[0] = text_info.position.x + glyph_width + x_offset;
      vert[1].pos[1] = text_info.position.y;
      vert[1].coord[0] = glyph_info.coord.top_right.x;
      vert[1].coord[1] = glyph_info.coord.lower_left.y;

      vert[2].pos[0] = text_info.position.x + x_offset;
      vert[2].pos[1] = text_info.position.y + glyph_height;
      vert[2].coord[0] = glyph_info.coord.lower_left.x;
      vert[2].coord[1] = glyph_info.coord.top_right.y;

      // tri2
      vert[3].pos[0] = text_info.position.x + glyph_width + x_offset;
      vert[3].pos[1] = text_info.position.y;
      vert[3].coord[0] = glyph_info.coord.top_right.x;
      vert[3].coord[1] = glyph_info.coord.lower_left.y;

      vert[4].pos[0] = text_info.position.x + glyph_width + x_offset;
      vert[4].pos[1] = text_info.position.y + glyph_height;
      vert[4].coord[0] = glyph_info.coord.top_right.x;
      vert[4].coord[1] = glyph_info.coord.top_right.y;

      vert[5].pos[0] = text_info.position.x + x_offset;
      vert[5].pos[1] = text_info.position.y + glyph_height;
      vert[5].coord[0] = glyph_info.coord.lower_left.x;
      vert[5].coord[1] = glyph_info.coord.top_right.y;

      x_offset += glyph_width + 0.005;
      vert += 6;  // go over the 6 verticies we just wrote
    }
  }

  // potentially create buffer on gpu (if we did not have one already)
  if (!text_vbo_id) {
    if (auto id = gl_buffer_mgr.Create()) {
      text_vbo_id = id;
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
  GLCall(glUseProgram(text_shader_program));
  GLCall(glUniform1i(glGetUniformLocation(text_shader_program, "texture1"), 0));
  // draw call
  GLCall(glDrawArrays(GL_TRIANGLES, 0, vertex_count));
}

}  // namespace quixotism
