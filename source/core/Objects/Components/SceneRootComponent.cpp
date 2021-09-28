#include "SceneRootComponent.h"
#include "core/VulkanDevice.h"

namespace Engine
{
    void SceneRootComponent::Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform)
    {
        RenderObject::Create(device, swapchain, uniform);
    }

    void SceneRootComponent::Render(float fDeltaTime, vk::CommandBuffer &commandBuffer)
    {
        RenderObject::Render(fDeltaTime, commandBuffer);
    }

    void SceneRootComponent::Update(float fDeltaTime)
    {
        RenderObject::Update(fDeltaTime);
    }
}