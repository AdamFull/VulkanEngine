#include "SceneBase.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine;
using namespace engine::core;
using namespace engine::core::scene;

void CSceneBase::attachObject(utl::ref_ptr<CRenderObject>& object)
{
    object->setParent(m_pRoot);
}