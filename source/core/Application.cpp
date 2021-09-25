#include "Application.h"

namespace Engine
{
    void Application::Create()
    {
        m_pWindow = std::make_unique<WindowHandle>();
        m_pWindow->Create(800, 600, "Vulkan");
        m_pInputMapper = std::make_unique<InputMapper>();
        m_pRender = std::make_unique<VulkanHighLevel>();
        m_pRender->Create(m_pWindow, "Vulkan", VK_MAKE_VERSION(1, 0, 0), "GENGINE", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);
    }

    void Application::LoadTexture(std::string srPath)
    {
        m_pRender->AddVulkanTexture(srPath, 0);
    }

    void Application::LoadMesh(std::string srPath)
    {
        m_pRender->AddVulkanMesh(srPath, {{}, {}, {1, 1, 1}});
    }

    void Application::CreatePipeline(const std::map<vk::ShaderStageFlagBits, std::string>& mShaders)
    {
        m_pRender->LoadShader(mShaders);
        m_pRender->CreateDescriptorSets();
        m_pRender->CreateGraphicsPipeline();
        m_pRender->CreateCommandBuffers();
    }

    void Application::run()
    {
        m_pRender->ValidateRunAbility();

        float delta_time{0.0f};
        while(!m_pWindow->IsShouldClose())
        {
            auto startTime = std::chrono::high_resolution_clock::now();

            m_pWindow->PollEvents();
            m_pInputMapper->Update(delta_time);
            m_pRender->DrawFrame(delta_time);

            auto currentTime = std::chrono::high_resolution_clock::now();
            delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        }
    }
}