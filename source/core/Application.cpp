#include "Application.h"
#include "graphics/VulkanHighLevel.h"
#include "keymapper/InputMapper.h"
#include "graphics/scene/objects/components/CameraManager.h"
#include "graphics/scene/SceneManager.h"
#include "filesystem/FilesystemHelper.h"

using namespace engine;
using namespace engine::controllers;
using namespace engine::core;
using namespace engine::core::render;
using namespace engine::core::window;
using namespace engine::core::scene;

void CApplication::create()
{
    CInputMapper::inst()->createAction("ServiceHandles", EActionKey::eEscape, EActionKey::eF1);
    CInputMapper::inst()->bindAction("ServiceHandles", EKeyState::eRelease, this, &CApplication::serviceHandle);

    FEngineCreateInfo createInfo = FilesystemHelper::getConfigAs<FEngineCreateInfo>("engine/config.json");
    UHLInstance->create(createInfo);

    m_pCameraController = utl::make_scope<CCameraEditorController>();
    m_pCameraController->create();

    CSceneManager::inst()->load(createInfo.engine.scene);
    CRenderSystem::inst()->create();
}

void CApplication::serviceHandle(EActionKey eKey, EKeyState eState)
{
    switch (eKey)
    {
    case EActionKey::eEscape:
    {
        CVulkanHighLevel::inst()->cleanup();
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
    while (!CWindowHandle::inst()->isShouldClose())
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        CWindowHandle::inst()->pollEvents();
        m_pCameraController->update(delta_time);

        currentScene->render(delta_time);

        // TODO: remove update from input mapper. Don't need anymore
        CInputMapper::inst()->update(delta_time);

        auto currentTime = std::chrono::high_resolution_clock::now();
        delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    }
}
