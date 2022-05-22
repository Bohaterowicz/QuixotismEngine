#include "component.hpp"
#include "transform.hpp"

std::unordered_map<const char *, component_type> component_rtti_manager::TypeMap = {};

REGISTER_COMPONENT(component);
REGISTER_COMPONENT(transform);