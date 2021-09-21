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
    //app.CreatePipeline(mShaders);
    app.run();
}