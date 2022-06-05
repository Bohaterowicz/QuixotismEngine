#pragma once
#include "entity.hpp"
#include "entity_manager.hpp"
#include "quixotism_c.hpp"
#include <string>
#include <vector>

class scene
{
  public:
    explicit scene(std::string SceneName) noexcept;

    template <class EntityType, typename... Args> size AddEntity(Args &&...params);

    entity *GetEntity(size Index)
    {
        return Entities[Index];
    }

    entity_block *GetEntities()
    {
        return Entities.GetRootBlock().get();
    }

  private:
    std::string Name{};
    entity_manager Entities;
};

template <class EntityType, typename... Args> size scene::AddEntity(Args &&...params)
{
    auto Index = Entities.Insert(std::make_unique<EntityType>(std::forward<Args>(params)...));
    return Index;
}