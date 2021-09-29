#include "Application.h"
#include "VulkanHighLevel.h"
#include "KeyMapping/InputMapper.h"

namespace Engine
{
    void Application::Create()
    {
        m_pWindow = std::make_unique<WindowHandle>();
        m_pWindow->Create(1920, 1080, "Vulkan");

        
        InputMapper::GetInstance()->CreateAction("ServiceHandles", EActionKey::eEscape, EActionKey::eF1);
        InputMapper::GetInstance()->BindAction("ServiceHandles", EKeyState::eRelease, this, &Application::ServiceHandle);

        VulkanHighLevel::GetInstance()->Create(m_pWindow, "Vulkan", VK_MAKE_VERSION(1, 0, 0), "GENGINE", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);

        m_pCameraController = std::make_unique<CameraEditorController>();
        m_pCameraController->Create();

        m_pRenderScene = std::make_unique<RenderScene>();
    }

    void Application::ServiceHandle(EActionKey eKey)
    {
        switch (eKey)
        {
        case EActionKey::eEscape: std::exit(10); break;
        default: break;
        }
    }
    /*
    чтобы попробовать запустить, нужно создать камеру,
    забиндить её в менеджер камеры, потом загрузить все ресурсы: материалы и мэши
    и потом уже аттачить их к сцене и стартовать. ПОка что костыльно. Потом нужно будет прикрутить менеджер ресов.
    */

    void Application::run()
    {
        m_pRenderScene->CreateObjects();
        float delta_time{0.0f};
        while(!m_pWindow->IsShouldClose())
        {
            auto startTime = std::chrono::high_resolution_clock::now();

            m_pWindow->PollEvents();
            m_pCameraController->Update(delta_time);

            m_pRenderScene->Render(delta_time);

            //TODO: remove update from input mapper. Don't need anymore
            InputMapper::GetInstance()->Update(delta_time);

            auto currentTime = std::chrono::high_resolution_clock::now();
            delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        }
    }
}