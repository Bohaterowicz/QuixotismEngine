#include "vertex_array_manager.hpp"

#include <GL/glew.h>

#include "gl_call.hpp"

namespace quixotism {

[[no_discard]] std::optional<VertexArrayID> VertexArrayManager::Create(
    const VertexBufferLayout &layout) {
  if (free_indicies.empty()) {
    return std::nullopt;
  }

  auto id = free_indicies.front();
  free_indicies.pop();
  assert(vao_array[id].gl_id == VertexArray::INVALID_VAO_ID);

  GLCall(glCreateVertexArrays(1, &vao_array[id].gl_id));
  assert(vao_array[id].gl_id != VertexArray::INVALID_VAO_ID);

  SpecifyAttributeLayout(id, layout);
  return id;
}

[[no_discard]] std::optional<VertexArray> VertexArrayManager::Get(
    VertexArrayID id) const {
  if (Exists(id)) {
    return vao_array[id];
  } else {
    return std::nullopt;
  }
}

bool VertexArrayManager::Exists(const VertexArrayID id) const {
  assert(id < ARRAY_SIZE);
  return vao_array[id].gl_id != VertexArray::INVALID_VAO_ID;
}

void VertexArrayManager::Destroy(const VertexArrayID id) {
  GLCall(glDeleteVertexArrays(1, &vao_array[id].gl_id));
  free_indicies.push(id);
}

VertexArrayManager::~VertexArrayManager() {
  for (const auto &vao : vao_array) {
    GLCall(glDeleteVertexArrays(1, &vao.gl_id));
  }
}

void VertexArrayManager::SpecifyAttributeLayout(
    const VertexArrayID id, const VertexBufferLayout &layout) {
  auto &vao = vao_array[id];
  u32 attribute_location = 0;
  vao.stride = 0;
  for (const auto &element : layout.GetLayoutElements()) {
    GLCall(glEnableVertexArrayAttrib(vao.gl_id, attribute_location));
    GLCall(glVertexArrayAttribFormat(vao.gl_id, attribute_location,
                                     element.count, element.type,
                                     static_cast<GLboolean>(element.normalize),
                                     static_cast<GLuint>(element.offset)));
    GLCall(glVertexArrayAttribBinding(vao.gl_id, attribute_location,
                                      element.binding_slot));
    ++attribute_location;
    vao.stride += element.count * element.type_size;
  }
}

}  // namespace quixotism
