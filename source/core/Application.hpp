#pragma once
#include "WindowHandle.h"
#include "InputMapper.h"
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
            m_pInputHandle = std::make_unique<InputMapper>(m_pWinHandle);
        }

        void CreateDevice();

        void CreatePipeline(const VulkanPipeline::shader_load_map_t& shaders);

        inline std::unique_ptr<InputMapper>& GetInputMapper() { return m_pInputHandle; }
        
        void run();
    private:
        std::unique_ptr<WindowHandle> m_pWinHandle;
        std::unique_ptr<InputMapper> m_pInputHandle;
        std::unique_ptr<VulkanDevice> m_pDevice;
        std::unique_ptr<VulkanPipeline> m_pPipeline;
    };
}