#include "Application.hpp"

namespace Engine
{
    void Application::CreatePipeline(const VulkanPipeline::shader_load_map_t& shaders)
    {
        if(!m_pPipeline)
        {
            m_pPipeline = std::unique_ptr<VulkanPipeline>();
            m_pPipeline->LoadShader(shaders);
        }
    }

    void Application::CreateDevice()
    {
        if(!m_pDevice)
        {
            m_pDevice = std::unique_ptr<VulkanDevice>(m_pWinHandle);
        }
    }


    void Application::run()
    {
        float deltaTime;
        while (!m_pWinHandle->ShouldClose()) 
        {
            auto startTime = std::chrono::high_resolution_clock::now();

            m_pWinHandle->PollEvents();

            auto currentTime = std::chrono::high_resolution_clock::now();
            deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        }
    }
}