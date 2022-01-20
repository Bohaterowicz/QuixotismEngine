#pragma once
#include "quixotism_c.hpp"

// SwapIntervalCount=0 means that vsync is disabled, =1 means vsync enabled, >1 means that we gonan skip every n-th
// buffer-swap. Usually we gonna use only 0 or 1 values, 1 for doing vsync on monitor refresh rate, 0 if we want to use
// custom framerate, like
//  on my monitor refresh rate is 144hz, if I want to run at 60 FPS i need to disable vsync and manually time the frame!
const uint32 SwapIntervalCount = 1;

// Window Init parameters
const int32 InitWindowPosX = 200;
const int32 InitWindowPosY = 200;
const int32 InitWindowClientWidth = 800;
const int32 InitWindowClientHeight = 600;

// Opengl init parameters
const int32 OPENGL_DESIRED_MAJOR_VERION = 4;
const int32 OPENGL_DESIRED_MINOR_VERION = 5;

// Pixel format
const uint32 PIXELFORMAT_COLOR_BITS = 32;
const uint32 PIXELFORMAT_ALPHA_BITS = 8;
const uint32 PIXELFORMAT_DEPTH_BITS = 24;
const uint32 PIXELFORMAT_STENCIL_BITS = 8;