#pragma once
#include "VulkanDevice.h"
#include "VulkanPipeline.h"

namespace Engine
{
    class Application
    {
    public:
        template<class ...Args>
        void CreateWindow(Args ...args)
        {
            m_pWinHandle = std::make_unique<WindowHandle>(std::forward<Args>(args)...);
        }

        void CreateDevice();

        void CreatePipeline(const VulkanPipeline::shader_load_map_t& shaders);
        
        void run();
    private:
        std::unique_ptr<WindowHandle> m_pWinHandle;
        std::unique_ptr<VulkanDevice> m_pDevice;
        std::unique_ptr<VulkanPipeline> m_pPipeline;
    };
}