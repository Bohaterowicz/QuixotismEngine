#include "transform.hpp"
#include <GLM/gtc/quaternion.hpp>
#include <GLM/gtx/quaternion.hpp>

COMPONENT_DEFINITION(transform, component);

const glm::vec3 transform::EngineForward = glm::vec3{1.0F, 0.0F, 0.0F};
const glm::vec3 transform::EngineRight = glm::vec3{0.0F, 0.0F, 1.0F};
const glm::vec3 transform::EngineUp = glm::vec3{0.0F, 1.0F, 0.0F};

glm::vec3 transform::GetForward()
{
    auto Pitch = glm::angleAxis(glm::radians(Rotation[0]), EngineRight);
    auto Yaw = glm::angleAxis(glm::radians(Rotation[1]), EngineUp);
    auto Roll = glm::angleAxis(glm::radians(Rotation[2]), EngineForward);

    auto RotationMatrix = glm::toMat3(glm::normalize(Yaw * Pitch * Roll));
    auto ForwardVector = EngineForward * RotationMatrix;
    return glm::normalize(ForwardVector);
}

glm::mat4 transform::GetTransformationMatrix()
{
    auto Pitch = glm::angleAxis(glm::radians(Rotation[0]), EngineRight);
    auto Yaw = glm::angleAxis(glm::radians(Rotation[1]), EngineUp);
    auto Roll = glm::angleAxis(glm::radians(Rotation[2]), EngineForward);

    auto RotationMatrix = glm::toMat4(glm::normalize(Yaw * Pitch * Roll));

    auto PositionMatrix = glm::mat4(1.0F);
    PositionMatrix[3] = glm::vec4(Position, 1.0F);

    auto ScaleMatrix = glm::mat4(1.0F);
    ScaleMatrix[0][0] *= Scale.x;
    ScaleMatrix[1][1] *= Scale.y;
    ScaleMatrix[2][2] *= Scale.z;

    return PositionMatrix * RotationMatrix * ScaleMatrix;
}