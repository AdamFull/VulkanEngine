#include "SceneRootComponent.h"
#include "Core/VulkanDevice.hpp"
#include "Resources/ResourceManager.h"

using namespace Engine::Core::Scene::Objects::Components;
using namespace Engine::Resources;

void SceneRootComponent::Create(std::shared_ptr<ResourceManager> resourceMgr)
{
    RenderObject::Create(resourceMgr);
}

void SceneRootComponent::Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    RenderObject::Render(commandBuffer, imageIndex);
}

void SceneRootComponent::Update(float fDeltaTime)
{
    RenderObject::Update(fDeltaTime);
}