#include "Application.hpp"

namespace Engine
{
    void Application::CreateDevice()
    {
        if(!m_pDevice)
        {
            m_pDevice = std::make_unique<VulkanDevice>(m_pWinHandle);
        }
    }

    void Application::CreatePipeline(const VulkanPipeline::shader_load_map_t& shaders)
    {
        if(!m_pPipeline)
        {
            int width, height;
            m_pWinHandle->GetWindowSize(&width, &height);

            m_pPipeline = std::make_unique<VulkanPipeline>(m_pDevice);
            m_pPipeline->LoadShader(shaders);
            m_pPipeline->CreatePipeline(VulkanPipeline::PipelineDefault(width, height));
        }
    }

    void Application::run()
    {
        float deltaTime;
        while (!m_pWinHandle->ShouldClose()) 
        {
            auto startTime = std::chrono::high_resolution_clock::now();

            m_pWinHandle->PollEvents();
            m_pInputHandle->Update(deltaTime);

            auto currentTime = std::chrono::high_resolution_clock::now();
            deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        }
    }
}