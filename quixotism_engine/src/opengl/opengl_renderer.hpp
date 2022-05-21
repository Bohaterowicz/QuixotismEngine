#pragma once
#include "quixotism_c.hpp"

class opengl_renderer
{
  public:
    opengl_renderer() noexcept;
    void ClearRenderTarget(int32 Width, int32 Height) noexcept;

    void SetClearColor(real32 Red, real32 Green, real32 Blue, real32 Alpha = 1.0);

  private:
};
