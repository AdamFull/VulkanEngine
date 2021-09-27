#include "SceneRootComponent.h"
#include "core/VulkanDevice.h"

namespace Engine
{
    void SceneRootComponent::Create(std::unique_ptr<Device> &device)
    {
        RenderObject::Create(device);
    }

    void SceneRootComponent::Render(float fDeltaTime, vk::CommandBuffer &commandBuffer)
    {
        RenderObject::Render(fDeltaTime, commandBuffer);
    }

    void SceneRootComponent::Update(float fDeltaTime, std::unique_ptr<SwapChain>& swapchain)
    {
        RenderObject::Update(fDeltaTime, swapchain);
    }
}