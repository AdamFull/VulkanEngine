#include "ResourceBase.h"

namespace Engine
{
    void ResourceBase::Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform)
    {
        m_pSwapChain = swapchain;
    }

    void ResourceBase::ReCreate(std::unique_ptr<Device>& device)
    {

    }

    void ResourceBase::Bind(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer)
    {

    }

    void ResourceBase::Destroy(std::unique_ptr<Device>& device)
    {

    }
}