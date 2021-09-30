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
        virtual void Update(uint32_t imageIndex);
        virtual void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
        virtual void Destroy();
    };
}
