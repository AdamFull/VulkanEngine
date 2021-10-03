#pragma once
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanSwapChain.h"
#include "Renderer/VulkanUniform.h"

namespace Engine
{
    enum class FResourceType 
    {
        eTexture2D,
        eTextureCubemap,
        eStaticMesh
    };

    class ResourceBase
    {
    public:
        virtual void ReCreate();
        virtual void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer);
        virtual void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
        virtual void Cleanup();
        virtual void Destroy();
    };
}
