#include "RenderScene.h"
#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "graphics/VulkanHighLevel.h"
#include "keymapper/InputMapper.h"
#include "graphics/overlay/ImguiOverlay.h"

#include "resources/ResourceCunstruct.h"

using namespace Engine;
using namespace Engine::Core;
using namespace Engine::Core::Scene;
using namespace Engine::Core::Scene::Objects::Components;
using namespace Engine::Resources;

RenderScene::~RenderScene()
{
    Destroy();
}

void RenderScene::Create()
{
    m_pRoot = std::make_shared<Core::Scene::Objects::Components::SceneRootComponent>();
    m_pResourceManager = std::make_shared<Resources::CResourceManager>();
    m_pResourceManager->create();
}

void RenderScene::ReCreate()
{
    m_pRoot->Cleanup();

    m_pRoot->ReCreate();
}

void RenderScene::Destroy()
{
    UDevice->GPUWait();
    /*if (URenderer->GetFrameStartFlag())
    {
        bool bResult;
        auto commandBuffer = URenderer->GetCurrentCommandBuffer();
        UHLInstance->EndFrame(commandBuffer, &bResult);
    }*/

    m_pRoot->Destroy();
}

void RenderScene::AttachObject(std::shared_ptr<Core::Scene::Objects::RenderObject> object)
{
    object->SetParent(m_pRoot);
}

void RenderScene::CreateObjects()
{
    UVBO->create();
    URenderer->create(m_pResourceManager, m_pRoot);
}

void RenderScene::Render(float fDeltaTime)
{
    m_pRoot->Update(fDeltaTime);

    //UOverlay->NewFrame();
    //UOverlay->Update(fDeltaTime);

    URenderer->render(m_pRoot);
}