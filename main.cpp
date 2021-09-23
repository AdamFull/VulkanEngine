#include "core/Application.hpp"

constexpr int iWidth = 800;
constexpr int iHeight = 600;
constexpr const char* srAppName = "Vulkan window";

Engine::Application app;
const Engine::Pipeline::SaderMap mShaders = 
{
    {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/vert.spv"},
    {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/frag.spv"},
};

int main()
{
    app.CreateApp(iWidth, iHeight, srAppName);
    app.run();
}