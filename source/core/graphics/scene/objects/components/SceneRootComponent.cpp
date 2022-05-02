#include "SceneRootComponent.h"
#include "graphics/VulkanDevice.hpp"
#include "resources/ResourceManager.h"

using namespace Engine::Core::Scene;
using namespace Engine::Resources;

void CSceneRootComponent::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    CRenderObject::create(renderPass, subpass);
}

void CSceneRootComponent::render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    CRenderObject::render(commandBuffer, imageIndex);
}

void CSceneRootComponent::update(float fDeltaTime)
{
    CRenderObject::update(fDeltaTime);
}