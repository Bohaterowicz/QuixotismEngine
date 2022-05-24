#include "camera.hpp"
#include "quixotism_engine.hpp"
#include "quixotism_math.hpp"

camera::camera() : entity{entity_type::CAMERA}
{
    AddComponent<transform>();
    InitPerspectiveProjectionDefault();
    InitOrthographicProjectionDefault();
    CurrentProjectionMtx = CalculateProjectionMatrix();
}

camera::camera(perspective_projection_info &Info) : entity{entity_type::CAMERA}
{
    AddComponent<transform>();
    SetPerspectiveProjectionInfo(Info);
    InitOrthographicProjectionDefault();
    CurrentProjectionMtx = CalculateProjectionMatrix();
}

camera::camera(orthographic_projection_info &Info)
    : entity{entity_type::CAMERA}, ProjectionType{projection_type::ORTHOGRAPHIC}
{
    AddComponent<transform>();
    SetOrthographicProjectionInfo(Info);
    InitPerspectiveProjectionDefault();
    CurrentProjectionMtx = CalculateProjectionMatrix();
}

void camera::ChangeProjectionType(projection_type Type)
{
    ProjectionType = Type;
    CurrentProjectionMtx = CalculateProjectionMatrix();
}

void camera::SetPerspectiveProjectionInfo(perspective_projection_info &Info)
{
    PerspectiveProjectionInfo = Info;
    if (ProjectionType == projection_type::PERSPECTIVE)
    {
        CurrentProjectionMtx = CalculateProjectionMatrix();
    }
}

void camera::SetOrthographicProjectionInfo(orthographic_projection_info &Info)
{
    OrthographicProjectionInfo = Info;
    if (ProjectionType == projection_type::ORTHOGRAPHIC)
    {
        CurrentProjectionMtx = CalculateProjectionMatrix();
    }
}

void camera::InitPerspectiveProjectionDefault()
{
    PerspectiveProjectionInfo.NearPlane = 0.01F;
    PerspectiveProjectionInfo.FarPlane = 1000.0F;
    PerspectiveProjectionInfo.FOV = 90.0F;
}

void camera::InitOrthographicProjectionDefault()
{
    auto const [Width, Height] = GetEngine()->GetWindowInfo();
    OrthographicProjectionInfo.NearPlane = 0.01F;
    OrthographicProjectionInfo.FarPlane = 1000.0F;
    OrthographicProjectionInfo.Left = 0.0F;
    OrthographicProjectionInfo.Bottom = 0.0F;
    OrthographicProjectionInfo.Top = static_cast<real32>(Width);
    OrthographicProjectionInfo.Right = static_cast<real32>(Height);
}

glm::mat4 camera::CalculateProjectionMatrix() const
{
    glm::mat4 Projection{};
    if (ProjectionType == projection_type::PERSPECTIVE)
    {
        Projection = CalculatePerspectiveProjectionMatrix();
    }
    else
    {
        Projection = ClaculateOrthographicProjectionMatrix();
    }
    return Projection;
}

glm::mat4 camera::CalculatePerspectiveProjectionMatrix() const
{
    glm::mat4 Projection = glm::mat4(0.0F);
    auto Scale = 1 / Tan((PerspectiveProjectionInfo.FOV / 2.0F) * (Pi32 / 180.0F));
    Projection[0][0] = Scale;
    Projection[1][1] = Scale;
    Projection[2][2] = -PerspectiveProjectionInfo.FarPlane /
                       (PerspectiveProjectionInfo.FarPlane - PerspectiveProjectionInfo.NearPlane);
    Projection[2][3] = -1.0F;
    Projection[3][2] = -(PerspectiveProjectionInfo.FarPlane * PerspectiveProjectionInfo.NearPlane) /
                       (PerspectiveProjectionInfo.FarPlane - PerspectiveProjectionInfo.NearPlane);

    return Projection;
}

glm::mat4 camera::ClaculateOrthographicProjectionMatrix() const
{
    glm::mat4 Projection = glm::mat4(0.0F);
    Projection[0][0] = 2.0F / (OrthographicProjectionInfo.Right - OrthographicProjectionInfo.Left);
    Projection[0][3] = -(OrthographicProjectionInfo.Right + OrthographicProjectionInfo.Left) /
                       (OrthographicProjectionInfo.Right - OrthographicProjectionInfo.Left);
    Projection[1][1] = 2.0F / (OrthographicProjectionInfo.Top - OrthographicProjectionInfo.Bottom);
    Projection[1][3] = -(OrthographicProjectionInfo.Top + OrthographicProjectionInfo.Bottom) /
                       (OrthographicProjectionInfo.Top - OrthographicProjectionInfo.Bottom);
    Projection[2][2] = -2.0F / (OrthographicProjectionInfo.FarPlane - OrthographicProjectionInfo.NearPlane);
    Projection[2][3] = -(OrthographicProjectionInfo.FarPlane + OrthographicProjectionInfo.NearPlane) /
                       (OrthographicProjectionInfo.FarPlane - OrthographicProjectionInfo.NearPlane);
    Projection[3][3] = 1.0F;

    return Projection;
}

glm::mat4 camera::GetViewMatrix()
{
    auto CameraTransform = GetComponent<transform>();
    auto ForwardVector = CameraTransform.GetForward();

    auto PositionTransform = glm::mat4(1.0F);
    PositionTransform[3] = glm::vec4(-CameraTransform.GetPosition(), 1.0F);

    auto RightVector = glm::normalize(glm::cross(ForwardVector, transform::EngineUp));
    auto LocalUpVector = glm::normalize(glm::cross(RightVector, ForwardVector));
    auto RotationTransform = glm::mat4(1.0F);
    RotationTransform[0] = glm::vec4(RightVector, 0.0F);
    RotationTransform[1] = glm::vec4(LocalUpVector, 0.0F);
    RotationTransform[2] = glm::vec4(-ForwardVector, 0.0F);
    RotationTransform = glm::transpose(RotationTransform);

    return RotationTransform * PositionTransform;
}

void camera::ProcessInput(engine_input &Input, real32 DeltaTime)
{
    auto Controller = Input.GetKeybaordController();
    auto &Transform = GetComponent<transform>();
    auto Speed = 1.0F; // m/s
    auto Movement = glm::vec3{0.0F};
    if (Controller.Up.EndedDown)
    {
        Movement += transform::EngineUp;
    }
    if (Controller.Down.EndedDown)
    {
        Movement += -transform::EngineUp;
    }
    if (Controller.Forward.EndedDown)
    {
        Movement += Transform.GetForward();
    }
    if (Controller.Backward.EndedDown)
    {
        Movement += -Transform.GetForward();
    }
    if (Controller.Right.EndedDown)
    {
        Movement += Transform.GetRight();
    }
    if (Controller.Left.EndedDown)
    {
        Movement += -Transform.GetRight();
    }
    if (glm::length(Movement) > 0.0F)
    {
        Movement = glm::normalize(Movement);
    }
    Transform.Move(Movement * Speed * DeltaTime);
}