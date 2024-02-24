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
  [[no_discard]] COMPONENT_TYPE& GetComponent(EntityId id) const {
    return elements[id].GetComponent<COMPONENT_TYPE>();
  }

 private:
};
using EntityId = EntityManager::IdType;

}  // namespace quixotism
