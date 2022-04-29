#include "SceneRootComponent.h"
#include "graphics/VulkanDevice.hpp"
#include "resources/ResourceManager.h"

using namespace Engine::Core::Scene::Objects::Components;
using namespace Engine::Resources;

void SceneRootComponent::Create(vk::RenderPass& renderPass, uint32_t subpass)
{
    RenderObject::Create(renderPass, subpass);
}

void SceneRootComponent::Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    RenderObject::Render(commandBuffer, imageIndex);
}

void SceneRootComponent::Update(float fDeltaTime)
{
    RenderObject::Update(fDeltaTime);
}