#pragma once
#include "components/component.hpp"
#include "quixotism_c.hpp"

#include <memory>
#include <vector>

class entity
{
  public:
    entity() = default;

    template <class ComponentType, typename... Args> bool32 AddComponent(Args &&...params);

    template <class ComponentType> ComponentType &GetComponent();

    template <class ComponentType> bool32 RemoveComponent();

    template <class ComponentType> std::vector<ComponentType *> GetComponents();

    template <class ComponentType> int32 RemoveComponents();

  private:
    std::vector<std::unique_ptr<component>> Components = {};
};

//***************
// entity::AddComponent
// perfect-forwards all params to the ComponentType constructor with the matching parameter list
// DEBUG: be sure to compare the arguments of this fn to the desired constructor to avoid perfect-forwarding failure
// cases EG: deduced initializer lists, decl-only static const int members, 0|NULL instead of nullptr, overloaded fn
// names, and bitfields
//***************
template <class ComponentType, typename... Args> bool32 entity::AddComponent(Args &&...params)
{
    if (&GetComponent<ComponentType>() == nullptr)
    {
        components.emplace_back(std::make_unique<ComponentType>(std::forward<Args>(params)...));
        return true;
    }
    else
    {
        return false;
    }
}

//***************
// entity::GetComponent
// returns the first component that matches the template type
// or that is derived from the template type
// EG: if the template type is Component, and components[0] type is BoxCollider
// then components[0] will be returned because it derives from Component
//***************
template <class ComponentType> ComponentType &entity::GetComponent()
{
    for (auto &&component : components)
    {
        if (component->IsClassType(ComponentType::Type))
            return *static_cast<ComponentType *>(component.get());
    }

    return *std::unique_ptr<ComponentType>(nullptr);
}

//***************
// entity::RemoveComponent
// returns true on successful removal
// returns false if components is empty, or no such component exists
//***************
template <class ComponentType> bool32 entity::RemoveComponent()
{
    if (components.empty())
        return false;

    auto &index =
        std::find_if(components.begin(), components.end(),
                     [classType = ComponentType::Type](auto &component) { return component->IsClassType(classType); });

    bool success = index != components.end();

    if (success)
        components.erase(index);

    return success;
}

//***************
// entity::GetComponents
// returns a vector of pointers to the the requested component template type following the same match criteria as
// GetComponent NOTE: the compiler has the option to copy-elide or move-construct componentsOfType into the return value
// here
// TODO(albert): pass in the number of elements desired (eg: up to 7, or only the first 2) which would allow a
// std::array return value, except there'd need to be a separate fn for getting them *all* if the user doesn't know how
// many such Components the entity has
// TODO(albert): define a GetComponentAt<ComponentType, int>() that can directly grab up to the the n-th component of
// the requested type
//***************
template <class ComponentType> std::vector<ComponentType *> entity::GetComponents()
{
    std::vector<ComponentType *> componentsOfType;

    for (auto &&component : components)
    {
        if (component->IsClassType(ComponentType::Type))
            componentsOfType.emplace_back(static_cast<ComponentType *>(component.get()));
    }

    return componentsOfType;
}

//***************
// entity::RemoveComponents
// returns the number of successful removals, or 0 if none are removed
//***************
template <class ComponentType> int32 entity::RemoveComponents()
{
    if (components.empty())
        return 0;

    int numRemoved = 0;
    bool success = false;

    do
    {
        auto &index =
            std::find_if(components.begin(), components.end(), [classType = ComponentType::Type](auto &component) {
                return component->IsClassType(classType);
            });

        success = index != components.end();

        if (success)
        {
            components.erase(index);
            ++numRemoved;
        }
    } while (success);

    return numRemoved;
}
