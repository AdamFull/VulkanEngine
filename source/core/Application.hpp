#pragma once
#include "VulkanMain.h"

namespace Engine
{
    class Application
    {
    public:
        void CreateWindow()
        {
            Main::CreateWindowHandle(engineCore, 800, 600, "Vulkan");
        }

        void CreateDevice();

        void CreatePipeline(const Pipeline::shader_load_map_t& shaders);
        
        void run();
    private:
        Main::FVulkanRenderInstance engineCore;
    };
}