#include "Application.h"
#include "graphics/VulkanHighLevel.h"
#include "keymapper/InputMapper.h"
#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "SceneFactory.h"
#include "filesystem/FilesystemHelper.h"

namespace Engine
{
    void Application::Create()
    {
        InputMapper::getInstance()->CreateAction("ServiceHandles", EActionKey::eEscape, EActionKey::eF1);
        InputMapper::getInstance()->BindAction("ServiceHandles", EKeyState::eRelease, this, &Application::ServiceHandle);

        Core::FEngineCreateInfo createInfo = FilesystemHelper::getConfigAs<Core::FEngineCreateInfo>("engine/config.json");
        UHLInstance->Create(createInfo);

        m_pCameraController = std::make_unique<Controllers::CameraEditorController>();
        m_pCameraController->Create();

        m_pRenderScene = SceneFactory::Create("scene.json");
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
            InputMapper::getInstance()->Update(delta_time);

            auto currentTime = std::chrono::high_resolution_clock::now();
            delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        }
    }
}