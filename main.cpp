#include "core/Application.hpp"

constexpr int iWidth = 800;
constexpr int iHeight = 600;
constexpr const char* srAppName = "Vulkan window";

Engine::Application app;
const Engine::VulkanPipeline::shader_load_map_t mShaders = 
{
    {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/vert.spv"},
    {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/frag.spv"},
};

int main()
{
    app.CreateWindow(iWidth, iHeight, srAppName);

    app.GetInputMapper()->BindAction(Engine::EActionKey::eW, Engine::EKeyState::ePress, [](){ std::cout << "W" << std::endl;});
    app.GetInputMapper()->BindAction(Engine::EActionKey::eW, Engine::EKeyState::ePressed, [](){ });
    app.GetInputMapper()->BindAction(Engine::EActionKey::eW, Engine::EKeyState::eRelease, [](){ std::cout << "oWo" << std::endl;});

    app.CreateDevice();
    app.CreatePipeline(mShaders);
    app.run();
}