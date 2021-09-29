#pragma once
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanUniform.h"

namespace Engine
{
    class ResourceBase
    {
    public:
        virtual void Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform);
        virtual void ReCreate(std::unique_ptr<Device>& device);
        virtual void Bind(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer);
        virtual void Destroy(std::unique_ptr<Device>& device);
    protected:
        std::shared_ptr<SwapChain> m_pSwapChain;
        std::shared_ptr<UniformBuffer> m_pUniform;
    };
}
