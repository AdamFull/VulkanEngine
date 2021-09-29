#pragma once
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanUniform.h"

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
        virtual void Create(std::string srResourcePath);
        virtual void ReCreate();
        virtual void Update(uint32_t imageIndex);
        virtual void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
        virtual void Destroy();
    };
}
