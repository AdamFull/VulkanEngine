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
    m_pRoot = utl::make_ref<CRenderObject>();
    UResources->create();
}

void CRenderScene::reCreate()
{
    m_pRoot->reCreate();
}

void CRenderScene::attachObject(utl::ref_ptr<CRenderObject>& object)
{
    object->setParent(m_pRoot);
}

void CRenderScene::createObjects()
{
    UVBO->create();
}

void CRenderScene::render(float fDeltaTime)
{
    m_pRoot->update(fDeltaTime);
    URenderer->render();
}