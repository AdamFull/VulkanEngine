#pragma once
#include "VulkanMain.h"

namespace Engine
{
    class Application
    {
    public:
        void CreateApp()
        {
            Main::CreateWindowHandle(engineCore, 800, 600, "Vulkan");
            Main::CreateInstance(engineCore);
            Main::SetupDebugMessenger(engineCore);
            Main::CreateSurface(engineCore);
            Main::CreateLogicalDevice(engineCore);
            Main::CreateSwapChain(engineCore);
            Main::CreateSwapChainImageViews(engineCore);
            Main::CreateRenderPass(engineCore);
            Main::CreateDescriptorSetLayout(engineCore);
            Main::CreateCommandPool(engineCore);
            Main::CreateMSAAResources(engineCore);
            Main::CreateDepthResources(engineCore);
            Main::CreateFramebuffers(engineCore);
            Main::CreateUniformBuffers(engineCore);
            Main::CreateDescriptorPool(engineCore);
            Main::CreateDescriptorSets(engineCore);
        }

        void CreateDevice();

        void CreatePipeline(const Pipeline::shader_load_map_t& shaders);
        
        void run();
    private:
        Main::FVulkanEngine engineCore;
    };
}