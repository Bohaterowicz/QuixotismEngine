#pragma once
#include "components/component.hpp"
#include "quixotism_c.hpp"
#include "quixotism_input.hpp"

#include <memory>
#include <vector>

enum class entity_type
{
    NO_TYPE,
    ACTOR,
    CAMERA,
    LIGHT,
    POSTPROCESS_VOLUME,
};

class entity
{
  public:
    explicit entity(entity_type Type = entity_type::NO_TYPE) : EntityType{Type}
    {
    }

    template <class ComponentType, typename... Args> bool32 AddComponent(Args &&...params);
    template <class ComponentType> ComponentType &GetComponent();
    template <class ComponentType> bool32 RemoveComponent();
    template <class ComponentType> std::vector<ComponentType *> GetComponents();
    template <class ComponentType> int32 RemoveComponents();

    _NODISCARD bool32 Is(entity_type Type) const
    {
        return EntityType == Type;
    }

    _NODISCARD entity_type GetType() const
    {
        return EntityType;
    }

    virtual void Update()
    {
    }

    virtual void ProcessInput(engine_input &Input, real32 DeltaTime)
    {
    }

  private:
    entity_type EntityType = entity_type::NO_TYPE;
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
        Components.emplace_back(std::make_unique<ComponentType>(std::forward<Args>(params)...));
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
    for (auto &&Component : Components)
    {
        if (Component->IsComponentType(ComponentType::Type))
            return *static_cast<ComponentType *>(Component.get());
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
    if (Components.empty())
        return false;

    auto &Index =
        std::find_if(Components.begin(), Components.end(), [ComponentType = ComponentType::Type](auto &Component) {
            return Component->IsComponentType(ComponentType);
        });

    bool Success = Index != Components.end();

    if (Success)
        Components.erase(Index);

    return Success;
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
    std::vector<ComponentType *> ComponentsOfType;

    for (auto &&Component : Components)
    {
        if (Component->IsComponentType(ComponentType::Type))
            ComponentsOfType.emplace_back(static_cast<ComponentType *>(Component.get()));
    }

    return ComponentsOfType;
}

//***************
// entity::RemoveComponents
// returns the number of successful removals, or 0 if none are removed
//***************
template <class ComponentType> int32 entity::RemoveComponents()
{
    if (Components.empty())
        return 0;

    int NumRemoved = 0;
    bool Success = false;

    do
    {
        auto &Index =
            std::find_if(Components.begin(), Components.end(), [ComponentType = ComponentType::Type](auto &Component) {
                return Component->IsClassType(ComponentType);
            });

        Success = Index != Components.end();

        if (Success)
        {
            Components.erase(Index);
            ++NumRemoved;
        }
    } while (Success);

    return NumRemoved;
}
