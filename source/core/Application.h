#pragma once

namespace Engine
{
    class Application
    {
    private:
        std::unique_ptr<WindowHandle> m_pWindow;
        std::unique_ptr<InputMapper> m_pInputMapper;
        std::unique_ptr<VulkanHighLevel> m_pRender;

    };
}