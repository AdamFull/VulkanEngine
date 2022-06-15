#include "RenderScene.h"
#include "graphics/scene/objects/components/CameraManager.h"
#include "graphics/VulkanHighLevel.h"
#include "keymapper/InputMapper.h"

#include "resources/ResourceCunstruct.h"

using namespace engine;
using namespace engine::core;
using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;

void CRenderScene::create()
{
    m_pRoot = make_ref<CRenderObject>();
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