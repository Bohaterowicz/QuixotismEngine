#pragma once
#include <array>
#include <queue>

#include "containers/bucket_array.hpp"
#include "core/entity.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

class EntityManager : public BucketArray<Entity> {
  using EntityId = IdType;

 public:
  CLASS_DELETE_COPY(EntityManager);
  EntityManager() = default;

  template <class COMPONENT_TYPE>
  [[no_discard]] COMPONENT_TYPE *GetComponent(EntityId id) const {
    return elements[id].GetComponent<COMPONENT_TYPE>();
  }

  IdType Add(Entity &&entity) {
    auto id = BucketArray<Entity>::Add(std::move(entity));
    if (id != INVALID_ID) {
      auto *e = Get(id);
      e->id = id;
    }
    return id;
  }

  IdType Clone(const IdType id) {
    auto *element = Get(id);
    if (!element) return INVALID_ID;
    auto cloned = *element;
    return Add(std::move(cloned));
  }

 private:
};
using EntityId = EntityManager::IdType;

}  // namespace quixotism
