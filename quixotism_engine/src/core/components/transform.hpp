#pragma once
#include "component.hpp"
#include "quixotism_c.hpp"
#include <glm/glm.hpp>

class transform : public component
{
    COMPONENT_DECLARATION();

  public:
    transform() = default;

    static const glm::vec3 EngineForward;
    static const glm::vec3 EngineRight;
    static const glm::vec3 EngineUp;

    void SetRotation(glm::vec3 NewRotation)
    {
        Rotation = NewRotation;
    }

    void SetPosition(glm::vec3 NewPosition)
    {
        Position = NewPosition;
    }

    void SetScale(glm::vec3 NewScale)
    {
        Scale = NewScale;
    }

    _NODISCARD const glm::vec3 &GetPosition() const
    {
        return Position;
    }

    _NODISCARD const glm::vec3 &GetRotation() const
    {
        return Rotation;
    }

    _NODISCARD const glm::vec3 &GetScale() const
    {
        return Scale;
    }

    void Move(glm::vec3 Delta)
    {
        Position += Delta;
    }

    void Rotate(glm::vec3 &EulerAngles);
    // void Rotate(glm::quat &Rotation);

    _NODISCARD glm::vec3 GetForward() const;
    _NODISCARD glm::vec3 GetRight() const;
    _NODISCARD glm::vec3 GetLocalUp() const;

    _NODISCARD glm::mat4 GetTransformationMatrix();

  private:
    glm::vec3 Position = {0.0F, 0.0F, 0.0F};
    glm::vec3 Rotation = {0.0F, 0.0F, 0.0F};
    glm::vec3 Scale = {1.0F, 1.0F, 1.0F};
};