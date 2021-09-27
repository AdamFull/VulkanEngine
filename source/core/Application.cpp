#include "Application.h"
#include "InputMapper.h"
#include "VulkanSwapChain.h"
#include "Objects/Controllers/CameraEditorController.h"
#include "Objects/Camera.h"

namespace Engine
{
    void Application::Create()
    {
        m_pWindow = std::make_unique<WindowHandle>();
        m_pWindow->Create(1920, 1080, "Vulkan");

        
        InputMapper::GetInstance()->CreateAction("ServiceHandles", EActionKey::eEscape, EActionKey::eF1);
        InputMapper::GetInstance()->BindAction("ServiceHandles", EKeyState::eRelease, this, &Application::ServiceHandle);

        m_pRoot = std::make_shared<SceneRootComponent>();

        auto camera_controller = std::make_shared<CameraEditorController>();
        camera_controller->SetParent(m_pRoot);

        m_pRender = std::make_unique<VulkanHighLevel>();
        m_pRender->Create(m_pWindow, "Vulkan", VK_MAKE_VERSION(1, 0, 0), "GENGINE", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);

        m_pRoot->Create(m_pRender->GetDevice());
    }

    void Application::LoadTexture(std::string srPath)
    {
        m_pRender->AddVulkanTexture(srPath, 0);
    }

    void Application::LoadMesh(std::string srPath)
    {
        m_pRender->AddVulkanMesh(srPath);
    }

    void Application::CreatePipeline(const std::map<vk::ShaderStageFlagBits, std::string>& mShaders)
    {
        m_pRender->AddPipeline(mShaders, PipelineConfig::CreateDefaultPipelineConfig(1920, 1080, vk::SampleCountFlagBits::e1));
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

            m_pRender->BeginFrame(delta_time);

            //TODO: remove update from input mapper. Don't need anymore
            InputMapper::GetInstance()->Update(delta_time);
            m_pRoot->Update(delta_time, m_pRender->GetSwapChain());
            m_pRoot->Render(delta_time, vk::CommandBuffer{});

            //TODO: bad practice
            auto cameraRaw = m_pRoot->GetChilds().front()->GetChilds().front();
            auto camera = std::static_pointer_cast<CameraBase>(cameraRaw);

            auto projectionView = camera->GetProjection() * camera->GetView();

            glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            m_pRender->GetUniformBuffer()->UpdateUniformBuffer(m_pRender->GetDevice(), m_pRender->GetCurrentImage(), projectionView * model);

            m_pRender->EndFrame();

            auto currentTime = std::chrono::high_resolution_clock::now();
            delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        }
    }
}