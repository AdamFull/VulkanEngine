#include "Application.h"
#include "graphics/VulkanHighLevel.h"
#include "keymapper/InputMapper.h"
#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "graphics/scene/SceneManager.h"
#include "filesystem/FilesystemHelper.h"

using namespace Engine;
using namespace Engine::Core;
using namespace Engine::Core::Scene;

void CApplication::create()
{
    CInputMapper::inst()->createAction("ServiceHandles", EActionKey::eEscape, EActionKey::eF1);
    CInputMapper::inst()->bindAction("ServiceHandles", EKeyState::eRelease, this, &CApplication::serviceHandle);

    Core::FEngineCreateInfo createInfo = FilesystemHelper::getConfigAs<Core::FEngineCreateInfo>("engine/config.json");
    UHLInstance->create(createInfo);

    m_pCameraController = std::make_unique<Controllers::CCameraEditorController>();
    m_pCameraController->create();

    CSceneManager::inst()->load("scene.json");
    CRenderSystem::inst()->create();
}

void CApplication::serviceHandle(EActionKey eKey, EKeyState eState)
{
    switch (eKey)
    {
    case EActionKey::eEscape:
    {
        CSceneManager::inst()->unload();
        std::exit(10);
    }
    break;
    default:
        break;
    }
}

void CApplication::run()
{
    auto& currentScene = CSceneManager::inst()->getScene();
    currentScene->createObjects();
    float delta_time{0.001f};
    while (!Core::Window::CWindowHandle::inst()->isShouldClose())
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        Core::Window::CWindowHandle::inst()->pollEvents();
        m_pCameraController->update(delta_time);

        currentScene->render(delta_time);

        // TODO: remove update from input mapper. Don't need anymore
        CInputMapper::inst()->update(delta_time);

        auto currentTime = std::chrono::high_resolution_clock::now();
        delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    }
}
