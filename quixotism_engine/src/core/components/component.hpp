#pragma once
#include "quixotism_c.hpp"

#include <unordered_map>

using component_type = size;

class component_rtti_manager
{
  public:
    inline static component_type TypeID = 0;
    static std::unordered_map<const char *, component_type> TypeMap;
    static component_type Register(const char *ComponentName)
    {
        auto ID = TypeID++;
        Assert(!TypeMap.contains(ComponentName));
        TypeMap[ComponentName] = ID;
        return ID;
    }
};

#define REGISTER_COMPONENT(ComponentClass)                                                            \
    const component_type ComponentClass::Type = component_rtti_manager::Register(#ComponentClass);

//****************
// COMPONENT_DECLARATION Macro
//
// This macro must be included in the declaration of any subclass of Component.
// It declares variables used in type checking.
//****************
#define COMPONENT_DECLARATION()                                                                                        \
  public:                                                                                                              \
    static const component_type Type;                                                                                  \
    virtual bool IsComponentType(component_type ComponentType) const override;
//****************

//****************
// COMPONENT_DEFINITION
//
// This macro must be included in the class definition to properly initialize
// variables used in type checking. Take special care to ensure that the
// proper parentclass is indicated or the run-time type information will be
// incorrect. Only works on single-inheritance RTTI.
//****************
#define COMPONENT_DEFINITION(ComponentClass, ParentComponent)                                                          \
    bool ComponentClass::IsComponentType(const component_type ComponentType) const                                     \
    {                                                                                                                  \
        if (ComponentType == ComponentClass::Type)                                                                     \
            return true;                                                                                               \
        return ParentComponent::IsComponentType(ComponentType);                                                        \
    }

//****************

class component // NOLINT
{
  public:
    static const component_type Type;

    component() = default;

    virtual ~component() = default;

    [[nodiscard]] virtual bool IsComponentType(const component_type ComponentType) const
    {
        return ComponentType == Type;
    }
};