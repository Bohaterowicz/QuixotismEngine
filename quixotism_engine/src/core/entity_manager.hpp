#pragma once
#include <array>
#include <queue>

#include "core/entity.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

using EntityId = u32;

class EntityManager {
 public:
  CLASS_DELETE_COPY(EntityManager);
  static constexpr size_t ARRAY_SIZE = 256;
  static constexpr EntityId INVALID_ID = 0;

  EntityManager() {
    for (u32 i = 1; i < ARRAY_SIZE; ++i) {
      free_ids.push(i);
    }
    Entity def{};
    entities.fill(def);
  }

  EntityId Add(const Entity&& entity) {
    if (free_ids.empty()) {
      return INVALID_ID;
    }

    auto id = free_ids.front();
    entities[id] = std::move(entity);
    free_ids.pop();
    return id;
  }

  [[no_discard]] std::optional<Entity> Get(EntityId id) const {}

  template <class COMPONENT_TYPE>
  [[no_discard]] COMPONENT_TYPE& GetComponent(EntityId id) const {
    return entities[id].GetComponent<COMPONENT_TYPE>();
  }

 private:
  std::queue<EntityId> free_ids;
  std::array<Entity, ARRAY_SIZE> entities;
};

}  // namespace quixotism
