#pragma once
#include "opengl_renderer.hpp"
#include "quixotism_c.hpp"
#include "quixotism_input.hpp"
#include "quixotism_platform_services.hpp"
#include "quixotism_scene.hpp"
#include "shader_program.hpp"
#include <memory>

struct quixotism_window_info
{
    int32 Width = 0;
    int32 Height = 0;
};

class quixotism_engine
{
  public:
    explicit quixotism_engine(quixotism_window_info &WindowInfo, platform_services &Platform) noexcept
        : PlatformServices{Platform}, Window{WindowInfo}
    {
        // This constructor should be left empty, unless something very special comes up, all initialization of the
        // engine should happen in Init(), since we must rely on that the engine instance is created so that the
        // GetEngine() function can return a value, which many of the other modules rely on.
    }

    void Init();

    quixotism_engine(quixotism_engine const &) = delete;
    quixotism_engine(quixotism_engine &&) = default;
    quixotism_engine &operator=(quixotism_engine const &) = delete;
    quixotism_engine &operator=(quixotism_engine &&) = default;
    ~quixotism_engine() = default;

    void UpdateAndRender(engine_input &Input, real32 DeltaTime) noexcept;
    void UpdateWindowInfo(quixotism_window_info &WindowInfo) noexcept
    {
        Window = WindowInfo;
    }
    platform_services PlatformServices = {};

    _NODISCARD real64 GetTime() const
    {
        return PlatformServices.GetTime();
    }

    _NODISCARD quixotism_window_info GetWindowInfo()
    {
        return Window;
    }

  private:
    quixotism_window_info Window = {};
    std::unique_ptr<opengl_renderer> Renderer = nullptr;
    std::unique_ptr<scene> CurrentScene = nullptr;

    shader_program Shader{};
    size ControlledCameraIndex = 0;
};

extern std::unique_ptr<quixotism_engine> QuixotismEngine;

inline quixotism_engine *GetEngine()
{
    return QuixotismEngine.get();
}