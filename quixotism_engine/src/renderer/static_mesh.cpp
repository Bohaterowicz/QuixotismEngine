#include "static_mesh.hpp"
#include "opengl_util.hpp"
#include <GL/glew.h>

static_mesh::static_mesh(std::shared_ptr<vertex_buffer> &VBO, std::shared_ptr<index_buffer> &IBO,
                         std::shared_ptr<vertex_array> &VAO)
    : VertexBuffer{VBO}, IndexBuffer{IBO}, VertexArray{VAO}
{
}

void static_mesh::Draw()
{
    VertexArray->Bind();
    VertexArray->BindVertexBuffer(VertexBuffer->GetGLID(), 0);
    VertexArray->BindIndexBuffer(IndexBuffer->GetGLID());
    GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));
}
