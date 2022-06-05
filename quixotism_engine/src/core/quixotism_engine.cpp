#include "quixotism_engine.hpp"
#include "camera.hpp"
#include "debug_out.hpp"
#include "gl_sampler.hpp"
#include "index_buffer.hpp"
#include "mesh_data.hpp"
#include "quiximg/src/quiximg.hpp"
#include "quixmesh/src/quixmesh.hpp"
#include "quixotism_math.hpp"
#include "static_mesh.hpp"
#include "stb_image.hpp"
#include "texture2d.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include "vertex_buffer_layout.hpp"

std::unique_ptr<quixotism_engine> QuixotismEngine;
std::unique_ptr<static_mesh> SMesh;
std::unique_ptr<texture2d> Texture1;
std::unique_ptr<gl_sampler> Sampler;

void quixotism_engine::Init()
{
    CurrentScene = std::make_unique<scene>("Default");
    Renderer = std::make_unique<opengl_renderer>();
    Renderer->SetClearColor(1.0, 0.0, 0.0);
    Shader.AddVertexAndShaderSoruce("../../quixotism_engine/data/shaders/no_transform.vert",
                                    "../../quixotism_engine/data/shaders/uniform_color.frag");
    Shader.CompileShader();

    auto FileReadResult = PlatformServices.ReadFile("../../quixotism_engine/data/meshes/cube.obj");
    auto Result = qmesh::ParseOBJ(FileReadResult.Content.get(), FileReadResult.Size);

    for (auto &Object : Result->Objects)
    {
        if (Object.NotTriangulated)
        {
            DEBUG_OUT("Mesh not triangulated, not laoding...");
            continue;
        }
        vertex_buffer_layout BufferLayout;
        BufferLayout.AddLayoutElementF(layout_element_type::POSITION, 3, false);
        BufferLayout.AddLayoutElementF(layout_element_type::TEXCOORD, 3, false);

        size BufferElementCount = 0;
        auto DataBuffer = SerializeDataByLayout(BufferLayout, &Object, &BufferElementCount);
        size BufferSize = BufferElementCount * sizeof(real32);
        auto IndexBuffer = GenerateIndexBuffer(Object.Faces.size());

        auto VBO = std::make_shared<vertex_buffer>();
        VBO->BufferData(DataBuffer.get(), BufferSize, STATIC_DRAW);

        auto IBO = std::make_shared<index_buffer>();
        IBO->BufferData(IndexBuffer.get(), Object.Faces.size() * 3 * sizeof(uint32), STATIC_DRAW);

        auto VAO = std::make_shared<vertex_array>(BufferLayout);

        SMesh = std::make_unique<static_mesh>(VBO, IBO, VAO);
    }

    auto ImageFile = PlatformServices.ReadFile("../../quixotism_engine/data/textures/wall_bmp.bmp");
    auto Image = qimg::LoadImage(ImageFile.Content.get(), ImageFile.Size);
    Assert(Image.Data);
    Texture1 = std::make_unique<texture2d>(Image.Data.get(), Image.Width, Image.Height, Image.NChannels);

    Sampler = std::make_unique<gl_sampler>();
    Sampler->SetBindSlot(0);
    Texture1->SetBindSlot(0);

    auto CameraEntityIndex = CurrentScene->AddEntity<camera>();
    ControlledCameraIndex = CameraEntityIndex;

    auto *CameraEntity = CurrentScene->GetEntity(CameraEntityIndex);

    auto &CameraTransform = CameraEntity->GetComponent<transform>();
    CameraTransform.SetPosition(glm::vec3(0.0F, 0.0F, -1.0F));
    CameraTransform.SetRotation(glm::vec3(0.0F, 90.0F, 0.0F));
}

void quixotism_engine::UpdateAndRender(engine_input &Input, real32 DeltaTime) noexcept
{
    // auto t = GetTime();
    // auto GreenValue = Sin(static_cast<real32>(t)) / 2.0F + 0.5F;
    // auto Color = glm::vec4(0.0F, GreenValue, 0.0F, 1.0F);

    auto *ControlledEntity = CurrentScene->GetEntity(ControlledCameraIndex);
    ControlledEntity->ProcessInput(Input, DeltaTime);
    auto *EntitiesBlock = CurrentScene->GetEntities();
    while (EntitiesBlock)
    {
        for (int32 Idx = 0; Idx < entity_block::BlockSize; ++Idx)
        {
            auto *Entity = EntitiesBlock->Data[Idx].get();
            if (Entity != nullptr)
            {
                Entity->Update();
            }
        }
        EntitiesBlock = EntitiesBlock->Next.get();
    }

    Renderer->ClearRenderTarget(Window.Width, Window.Height);
    Shader.Bind();
    // Shader.SetUniform4f("Color", Color);
    Shader.SetUniform1i("TextureSampler", 0);
    auto *Camera = static_cast<camera *>(CurrentScene->GetEntity(ControlledCameraIndex));
    auto MVP = Camera->GetProjectionMatrix() * Camera->GetViewMatrix();
    Shader.SetUniformMtx4("MVP", MVP);
    Sampler->Bind();
    Texture1->Bind();
    SMesh->Draw();
}