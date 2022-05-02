#include "Application.h"
#include "graphics/VulkanHighLevel.h"
#include "keymapper/InputMapper.h"
#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "SceneFactory.h"
#include "filesystem/FilesystemHelper.h"

namespace Engine
{
    void CApplication::create()
    {
        CInputMapper::getInstance()->createAction("ServiceHandles", EActionKey::eEscape, EActionKey::eF1);
        CInputMapper::getInstance()->bindAction("ServiceHandles", EKeyState::eRelease, this, &CApplication::serviceHandle);

        Core::FEngineCreateInfo createInfo = FilesystemHelper::getConfigAs<Core::FEngineCreateInfo>("engine/config.json");
        UHLInstance->create(createInfo);

        m_pCameraController = std::make_unique<Controllers::CCameraEditorController>();
        m_pCameraController->create();

        m_pRenderScene = CSceneFactory::create("scene.json");
    }

    void CApplication::serviceHandle(EActionKey eKey, EKeyState eState)
    {
        switch (eKey)
        {
        case EActionKey::eEscape: 
        {
            m_pRenderScene->destroy();
            std::exit(10); 
        }break;
        default: break;
        }
    }

    void CApplication::run()
    {
        m_pRenderScene->createObjects();
        float delta_time{0.001f};
        while(!UWinHandle->isShouldClose())
        {
            auto startTime = std::chrono::high_resolution_clock::now();

            UWinHandle->pollEvents();
            m_pCameraController->update(delta_time);

            m_pRenderScene->render(delta_time);

            //TODO: remove update from input mapper. Don't need anymore
            CInputMapper::getInstance()->update(delta_time);

            auto currentTime = std::chrono::high_resolution_clock::now();
            delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        }
    }
}