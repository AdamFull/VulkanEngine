#include "VulkanGenerator.h"

namespace Engine
{
    void Create()
    {

    }

    void GeneratorBase::ReCreate()
    {

    }

    void GeneratorBase::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        ResourceBase::Update(imageIndex, pUniformBuffer);
    }

    void GeneratorBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        ResourceBase::Bind(commandBuffer, imageIndex);
    }

    void GeneratorBase::Cleanup()
    {
        ResourceBase::Cleanup();
    }

    void GeneratorBase::Destroy()
    {
        ResourceBase::Destroy();
    }

}