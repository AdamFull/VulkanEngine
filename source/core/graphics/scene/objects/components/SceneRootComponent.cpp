#include "SceneRootComponent.h"
#include "graphics/VulkanDevice.hpp"
#include "resources/ResourceManager.h"

using namespace Engine::Core::Scene;
using namespace Engine::Resources;

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