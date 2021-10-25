#include "PrefiltredGenerator.h"

namespace Engine
{
    void PrefiltredGenerator::Create()
    {
        GeneratorBase::Create();
    }

    void PrefiltredGenerator::ReCreate()
    {
        GeneratorBase::ReCreate();
    }

    void PrefiltredGenerator::Update(uint32_t imageIndex)
    {
        GeneratorBase::Update(imageIndex, pUniformBuffer);
    }

    void PrefiltredGenerator::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        GeneratorBase::Bind(commandBuffer, imageIndex);
    }

    void PrefiltredGenerator::Cleanup()
    {
        GeneratorBase::Cleanup();
    }

    void PrefiltredGenerator::Destroy()
    {
        GeneratorBase::Destroy();
    }

    void PrefiltredGenerator::CreateRenderPass()
    {
        GeneratorBase::CreateRenderPass();
    }

    void PrefiltredGenerator::CreateFramebuffer()
    {
        GeneratorBase::CreateFramebuffer();
    }

    void PrefiltredGenerator::CreateDescriptorSets(uint32_t images)
    {
        GeneratorBase::CreateDescriptorSets(images);
    }
}