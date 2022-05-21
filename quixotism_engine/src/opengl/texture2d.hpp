#pragma once
#include "gl_texture.hpp"
#include "quixotism_c.hpp"

#include <memory>

class texture2d : public gl_texture
{
  public:
    texture2d(std::unique_ptr<uint8> ImageData, int32 Width, int32 Height, int32 ChannelCount);
    texture2d(uint8 *ImageData, int32 Width, int32 Height, int32 ChannelCount);

  private:
};