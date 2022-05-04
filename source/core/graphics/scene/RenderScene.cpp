#include "RenderScene.h"
#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "graphics/VulkanHighLevel.h"
#include "keymapper/InputMapper.h"
#include "graphics/overlay/ImguiOverlay.h"

#include "resources/ResourceCunstruct.h"

using namespace Engine;
using namespace Engine::Core;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;

CRenderScene::~CRenderScene()
{
    destroy();
}

void CRenderScene::create()
{
    m_pRoot = std::make_shared<Core::Scene::CSceneRootComponent>();
    CResourceManager::getInstance()->create();
}

void CRenderScene::reCreate()
{
    m_pRoot->cleanup();
    m_pRoot->reCreate();
}

void CRenderScene::destroy()
{
    CDevice::getInstance()->GPUWait();
    /*if (CRenderSystem::getInstance()->GetFrameStartFlag())
    {
        bool bResult;
        auto commandBuffer = CRenderSystem::getInstance()->GetCurrentCommandBuffer();
        UHLInstance->EndFrame(commandBuffer, &bResult);
    }*/

    m_pRoot->destroy();
}

void CRenderScene::attachObject(std::shared_ptr<Core::Scene::CRenderObject> object)
{
    object->setParent(m_pRoot);
}

void CRenderScene::createObjects()
{
    CVBO::getInstance()->create();
}

void CRenderScene::render(float fDeltaTime)
{
    m_pRoot->update(fDeltaTime);

    CImguiOverlay::getInstance()->newFrame();
    CImguiOverlay::getInstance()->update(fDeltaTime);

    CRenderSystem::getInstance()->render();
}