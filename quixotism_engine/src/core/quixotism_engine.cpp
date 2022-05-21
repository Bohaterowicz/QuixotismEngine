#include "quixotism_engine.hpp"
#include "gl_sampler.hpp"
#include "index_buffer.hpp"
#include "mesh_data.hpp"
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
    Renderer = std::make_unique<opengl_renderer>();
    Renderer->SetClearColor(1.0, 0.0, 0.0);
    Shader.AddVertexAndShaderSoruce("../../quixotism_engine/data/shaders/no_transform.vert",
                                    "../../quixotism_engine/data/shaders/uniform_color.frag");
    Shader.CompileShader();

    float Vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int Indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    auto VertPtr = std::make_unique<real32[]>(ArrayCount(Vertices));
    memcpy(VertPtr.get(), &Vertices, sizeof(Vertices));

    auto IndPtr = std::make_unique<uint32[]>(6);
    memcpy(IndPtr.get(), &Indices, sizeof(Indices));

    auto MeshData = std::make_shared<mesh_data>();
    MeshData->Position = std::move(VertPtr);
    MeshData->Indices = std::move(IndPtr);
    MeshData->IndexCount = static_cast<uint32>(ArrayCount(Indices));

    vertex_buffer_layout BufferLayout;
    BufferLayout.AddLayoutElementF(3, "Pos", false);
    BufferLayout.AddLayoutElementF(3, "VertexColor", false);
    BufferLayout.AddLayoutElementF(2, "TexCoords", false);

    auto VBO = std::make_shared<vertex_buffer>();
    VBO->BufferData(MeshData->Position.get(), sizeof(Vertices), STATIC_DRAW);

    auto IBO = std::make_shared<index_buffer>();
    IBO->BufferData(MeshData->Indices.get(), sizeof(Indices), STATIC_DRAW);

    auto VAO = std::make_shared<vertex_array>(BufferLayout);

    SMesh = std::make_unique<static_mesh>(MeshData, VBO, IBO, VAO);

    int32 Width = 0;
    int32 Height = 0;
    int32 NChannels = 0;
    uint8 *ImageData = stbi_load("../../quixotism_engine/data/textures/wall.jpg", &Width, &Height, &NChannels, 0);
    Texture1 = std::make_unique<texture2d>(ImageData, Width, Height, NChannels);
    stbi_image_free(ImageData);

    Sampler = std::make_unique<gl_sampler>();
    Sampler->SetBindSlot(0);
    Texture1->SetBindSlot(0);
}

void quixotism_engine::UpdateAndRender() noexcept
{
    // auto t = GetTime();
    // auto GreenValue = Sin(static_cast<real32>(t)) / 2.0F + 0.5F;
    // auto Color = glm::vec4(0.0F, GreenValue, 0.0F, 1.0F);

    Renderer->ClearRenderTarget(Window.Width, Window.Height);
    Shader.Bind();
    // Shader.SetUniform4f("Color", Color);
    Shader.SetUniform1i("TextureSampler", 0);
    Sampler->Bind();
    Texture1->Bind();
    SMesh->Draw();
}