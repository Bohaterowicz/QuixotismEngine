#pragma once

#include "core/components/component.hpp"
#include "core/components/component_manager.hpp"
#include "quixotism_c.hpp"
#include "transform.hpp"

namespace quixotism {

class Entity {
 public:
  Entity() = default;

  template <class COMPONENT_TYPE>
  bool AddComponent(COMPONENT_TYPE& component) {
    auto id = ComponentManager::GetInstance().Add(component);
    if (id != ComponentManager::INVALID_ID) {
      components[COMPONENT_TYPE::Type()] = id;
      return true;
    }
    return false;
  }

  template <class COMPONENT_TYPE>
  COMPONENT_TYPE* GetComponent() const {
    auto id = components[COMPONENT_TYPE::Type()];
    if (id != ComponentManager::INVALID_ID) {
      return ComponentManager::GetInstance().GetComponent<COMPONENT_TYPE>(id);
    } else {
      return nullptr;
    }
  }

  Transform transform;

 private:
  ComponentId components[COMPONENT_COUNT] = {};
};

}  // namespace quixotism
