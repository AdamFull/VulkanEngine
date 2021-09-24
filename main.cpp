#include "core/Application.h"

const std::vector<Render::Vertex> vertices = 
{
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
    {{0.5f,  -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    {{0.5f,  0.5f,  0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f,  0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
    {{0.5f,  -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    {{0.5f,  0.5f,  -0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f,  -0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint32_t> indices = 
{
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
};

int main()
{
    Engine::PEngine()->CreateWindowHandle(800, 800, "Vulkan");
    Engine::PEngine()->InitializeVulkanBaseline(
        "Vulkan",
        VK_MAKE_VERSION(1, 0, 0),
        "GENGINE",
        VK_MAKE_VERSION(1, 0, 0),
        VK_API_VERSION_1_0
    );

    Engine::shader_list_t vShList = 
    {
        {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/vert.spv"},
        {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/frag.spv"}
    };

    Engine::PEngine()->InitializeVulkanPipeline("main", vShList);
    Engine::PEngine()->RegisterVulkanTexture("../../assets/textures/viking_room.png", 0);
    Engine::PEngine()->LoadStaticMesh("../../assets/meshes/viking_room.obj", {{0.f, 0.f, 0.f},{1.f, 1.f, 1.f},{1.f, 1.f, 1.f}});

    try
    {
        Engine::PEngine()->RunMainLoop();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}