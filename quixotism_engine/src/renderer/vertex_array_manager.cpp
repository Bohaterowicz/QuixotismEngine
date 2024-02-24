#include "vertex_array_manager.hpp"

#include <GL/glew.h>

#include "gl_call.hpp"

namespace quixotism {

std::optional<VertexArrayID> VertexArrayManager::Create(
    const VertexBufferLayout &layout) {
  if (free_indicies.empty()) {
    return std::nullopt;
  }

  auto id = free_indicies.front();
  assert(vao_array[id].id == VertexArray::INVALID_VAO_ID);

  GLCall(glCreateVertexArrays(1, &vao_array[id].id));
  assert(vao_array[id].id != VertexArray::INVALID_VAO_ID);

  SpecifyAttributeLayout(id, layout);
  free_indicies.pop();
  return id;
}

std::optional<VertexArray> VertexArrayManager::Get(VertexArrayID id) const {
  if (Exists(id)) {
    return vao_array[id];
  } else {
    return std::nullopt;
  }
}

bool VertexArrayManager::Exists(const VertexArrayID id) const {
  assert(id < ARRAY_SIZE);
  return vao_array[id].id != VertexArray::INVALID_VAO_ID;
}

void VertexArrayManager::Destroy(const VertexArrayID id) {
  GLCall(glDeleteVertexArrays(1, &vao_array[id].id));
  free_indicies.push(id);
}

VertexArrayManager::~VertexArrayManager() {
  for (const auto &vao : vao_array) {
    GLCall(glDeleteVertexArrays(1, &vao.id));
  }
}

void VertexArrayManager::SpecifyAttributeLayout(
    const VertexArrayID id, const VertexBufferLayout &layout) {
  auto &vao = vao_array[id];
  u32 attribute_location = 0;
  vao.stride = 0;
  for (const auto &element : layout.GetLayoutElements()) {
    GLCall(glEnableVertexArrayAttrib(vao.id, attribute_location));
    GLCall(glVertexArrayAttribFormat(vao.id, attribute_location, element.count,
                                     element.type,
                                     static_cast<GLboolean>(element.normalize),
                                     static_cast<GLuint>(element.offset)));
    GLCall(glVertexArrayAttribBinding(vao.id, attribute_location,
                                      element.binding_slot));
    ++attribute_location;
    vao.stride += element.count * element.type_size;
  }
  vao.layout = layout;
}

}  // namespace quixotism
