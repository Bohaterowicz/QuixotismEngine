#include "quixotism_scene.hpp"

scene::scene(const std::string SceneName) noexcept : Name{std::move(SceneName)}
{
    // zero entity...
    AddEntity<entity>();
}