#include "quixotism_renderer.hpp"

#include "GL/glew.h"
#include "core/quixotism_engine.hpp"
#include "dbg_print.hpp"
#include "gl_call.hpp"

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

void QuixotismRenderer::Test() {
  static bool init = false;
  if (!init) {
    init = true;

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
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

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
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void QuixotismRenderer::PushText(std::string &&text, Vec2 coord) {
  TextDrawInfo info;
  info.text = std::move(text);
  info.coord = coord;
  draw_text_queue.push_back(std::move(info));
}

struct GlyphVert {
  r32 pos[2];
  r32 coord[2];
};

void QuixotismRenderer::DrawText() {
  // first compute the buffer size needed to store vertex data or all characters for all text to render
  size_t char_count = 0;
  for (const auto &text_info : draw_text_queue) {
    char_count += text_info.text.size();
  }

  // for now we hard code that we use 6 verts per character (quad = 2 triangles) with 4 floats per vertex (x, y, u, v)
  size_t required_buffer_size = char_count * 6 * 4 * sizeof(r32);
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

  assert(cached_text_vert_buffer && cached_text_vert_buffer_size >= required_buffer_size);

  // now we fill the vertex data buffer
  auto &font = QuixotismEngine::GetEngine().font;
  auto *vert = reinterpret_cast<GlyphVert *>(cached_text_vert_buffer.get());
  for (const auto &text_info : draw_text_queue) {
    r32 x_offset = 0;
    for (const auto &c: text_info.text) {
      auto &glyph_info = font.GetGlyphInfo(c);
      vert->pos[0] = text_info.coord.x + x_offset;
      vert->pos[1] = text_info.coord.y;
      x_offset += 
    }
  }

  // potentially create buffer on gpu (if we did nto have one already)

  // send vert data to gpu
  
  // bind vert buffer

  // setup shader

  // draw call
}

}  // namespace quixotism
