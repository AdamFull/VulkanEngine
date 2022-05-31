#include "RenderScene.h"
#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "graphics/VulkanHighLevel.h"
#include "keymapper/InputMapper.h"
#include "graphics/editor/EditorUI.h"

#include "resources/ResourceCunstruct.h"

using namespace Engine;
using namespace Engine::Core;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;

void CRenderScene::create()
{
    m_pRoot = make_ref<CSceneRootComponent>();
    CResourceManager::inst()->create();
}

void CRenderScene::reCreate()
{
    m_pRoot->reCreate();
}

void CRenderScene::cleanup()
{
    if(m_pRoot)
        m_pRoot->cleanup();
}

void CRenderScene::attachObject(ref_ptr<CRenderObject>& object)
{
    object->setParent(m_pRoot);
}

void CRenderScene::createObjects()
{
    CVBO::inst()->create();
}

void CRenderScene::render(float fDeltaTime)
{
    m_pRoot->update(fDeltaTime);
    CRenderSystem::inst()->render();
}