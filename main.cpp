#include "core/Application.h"

int main()
{
    Engine::Application app;
    app.Create();

    std::map<vk::ShaderStageFlagBits, std::string> vShList = 
    {
        {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/vert.spv"},
        {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/frag.spv"}
    };

    app.LoadTexture("../../assets/textures/viking_room.png");
    app.LoadMesh("../../assets/meshes/viking_room.obj");

    app.CreatePipeline(vShList);

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}