#include "transform.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

COMPONENT_DEFINITION(transform, component);

const glm::vec3 transform::EngineForward = glm::vec3{1.0F, 0.0F, 0.0F};
const glm::vec3 transform::EngineRight = glm::vec3{0.0F, 0.0F, 1.0F};
const glm::vec3 transform::EngineUp = glm::vec3{0.0F, 1.0F, 0.0F};

glm::vec3 transform::GetForward()
{
    auto RotationMatrix = glm::mat3(glm::quat(glm::radians(Rotation)));
    auto Forward = EngineForward * RotationMatrix;
    return glm::normalize(Forward);
}

glm::vec3 transform::GetRight()
{
    auto Forward = GetForward();
    auto RightVector = glm::cross(Forward, EngineUp);
    return glm::normalize(RightVector);
}

glm::vec3 transform::GetLocalUp()
{
    auto Forward = GetForward();
    auto RightVector = glm::normalize(glm::cross(Forward, EngineUp));
    auto LocalUpVector = glm::cross(RightVector, Forward);
    return glm::normalize(LocalUpVector);
}

glm::mat4 transform::GetTransformationMatrix()
{
    auto Pitch = glm::angleAxis(glm::radians(Rotation.x), EngineRight);
    auto Yaw = glm::angleAxis(glm::radians(Rotation.y), EngineUp);
    auto Roll = glm::angleAxis(glm::radians(Rotation.z), EngineForward);

    auto RotationMatrix = glm::toMat4(glm::normalize(Yaw * Pitch * Roll));

    auto PositionMatrix = glm::mat4(1.0F);
    PositionMatrix[3] = glm::vec4(Position, 1.0F);

    auto ScaleMatrix = glm::mat4(1.0F);
    ScaleMatrix[0][0] *= Scale.x;
    ScaleMatrix[1][1] *= Scale.y;
    ScaleMatrix[2][2] *= Scale.z;

    return PositionMatrix * RotationMatrix * ScaleMatrix;
}

void transform::Rotate(glm::vec3 EulerAnglesDelta)
{
    Rotation += EulerAnglesDelta;
}