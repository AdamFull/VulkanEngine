#include "SceneRootComponent.h"
#include "Renderer/VulkanDevice.h"

namespace Engine
{
    void SceneRootComponent::Create()
    {
        RenderObject::Create();
    }

    void SceneRootComponent::Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
    {
        RenderObject::Render(commandBuffer, imageIndex);
    }

    void SceneRootComponent::Update(float fDeltaTime)
    {
        RenderObject::Update(fDeltaTime);
    }
}