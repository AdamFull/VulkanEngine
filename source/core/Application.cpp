#include "Application.h"
#include "Renderer/VulkanHighLevel.h"
#include "KeyMapping/InputMapper.h"
#include "Objects/Components/Camera/CameraComponent.h"
#include "Objects/Components/Camera/CameraManager.h"
#include "SceneFactory.h"

namespace Engine
{
    void Application::Create()
    {
        InputMapper::GetInstance()->CreateAction("ServiceHandles", EActionKey::eEscape, EActionKey::eF1);
        InputMapper::GetInstance()->BindAction("ServiceHandles", EKeyState::eRelease, this, &Application::ServiceHandle);

        FEngineCreateInfo createInfo;
        createInfo.window.width = 1920;
        createInfo.window.height = 1080;
        createInfo.window.name = "Vulkan";
        createInfo.appName = "Vulkan";
        createInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
        createInfo.engineName = "IncenerateEngine";
        createInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        createInfo.apiVersion = VK_API_VERSION_1_0;
        UHLInstance->Create(createInfo);

        m_pCameraController = std::make_unique<CameraEditorController>();
        m_pCameraController->Create();

        m_pRenderScene = SceneFactory::Create("../../assets/scene.json");
    }

    void Application::ServiceHandle(EActionKey eKey, EKeyState eState)
    {
        switch (eKey)
        {
        case EActionKey::eEscape: 
        {
            m_pRenderScene->Destroy();
            std::exit(10); 
        }break;
        default: break;
        }
    }

    void Application::run()
    {
        m_pRenderScene->CreateObjects();
        float delta_time{0.001f};
        while(!UWinHandle->IsShouldClose())
        {
            auto startTime = std::chrono::high_resolution_clock::now();

            UWinHandle->PollEvents();
            m_pCameraController->Update(delta_time);

            m_pRenderScene->Render(delta_time);

            //TODO: remove update from input mapper. Don't need anymore
            InputMapper::GetInstance()->Update(delta_time);

            auto currentTime = std::chrono::high_resolution_clock::now();
            delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        }
    }
}