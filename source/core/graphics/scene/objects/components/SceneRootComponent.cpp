#include "SceneRootComponent.h"
#include "graphics/VulkanDevice.hpp"
#include "resources/ResourceManager.h"

using namespace engine::core::scene;
using namespace engine::resources;

void CSceneRootComponent::create()
{
    CRenderObject::create();
}

void CSceneRootComponent::render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    CRenderObject::render(commandBuffer, imageIndex);
}

void CSceneRootComponent::update(float fDeltaTime)
{
    CRenderObject::update(fDeltaTime);
}