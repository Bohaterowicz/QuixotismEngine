#include "opengl_renderer.hpp"
#include "opengl_util.hpp"
#include <GL/glew.h>

opengl_renderer::opengl_renderer() noexcept
{
    GLCall(glEnable(GL_DEPTH_TEST));
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    GLCall(glClearColor(0.5F, 0.5F, 0.5F, 1.0F));
    GLCall(glClearDepth(1.0F));
    GLCall(glClearStencil(0));
    GLDEBUG("Specified clear colors");
}

void opengl_renderer::ClearRenderTarget(int32 Width, int32 Height) noexcept
{
    GLCall(glViewport(0, 0, Width, Height));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

void opengl_renderer::SetClearColor(real32 Red, real32 Green, real32 Blue, real32 Alpha)
{
    GLCall(glClearColor(Red, Green, Blue, Alpha));
}