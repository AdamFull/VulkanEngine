#include "Application.h"
#include "Camera.h"

namespace Engine
{
    void Application::Create()
    {
        m_pWindow = std::make_unique<WindowHandle>();
        m_pWindow->Create(1920, 1080, "Vulkan");

        m_pInputMapper = std::make_unique<InputMapper>();
        m_pInputMapper->CreateAction("ServiceHandles", EActionKey::eEscape, EActionKey::eF1);
        m_pInputMapper->BindAction("ServiceHandles", EKeyState::eRelease, this, &Application::ServiceHandle);

        m_pCameraController = std::make_unique<CameraController>();
        m_pCameraController->Initialize(m_pInputMapper);

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
        m_pRender->AddPipeline(mShaders, PipelineBase::CreateDefaultPipelineConfig(1920, 1080, vk::SampleCountFlagBits::e1));
        m_pRender->CreateCommandBuffers();
    }

    void Application::ServiceHandle(EActionKey eKey)
    {
        switch (eKey)
        {
        case EActionKey::eEscape: std::exit(10); break;
        default: break;
        }
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
            m_pCameraController->Update(delta_time);

            auto aspectRatio = m_pRender->GetAspect();
            auto camera = m_pCameraController->GetCamera();
            camera->SetPerspectiveProjection(glm::radians(90.f), aspectRatio, 0.1f, 50.f);
            auto projectionView = camera->GetProjection() * camera->GetView();
            m_pRender->SetProjectionView(projectionView);

            m_pRender->DrawFrame(delta_time);

            auto currentTime = std::chrono::high_resolution_clock::now();
            delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        }
    }
}