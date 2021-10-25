#include "BRDFLUTGenerator.h"

namespace Engine
{
    void BRDFLUTGenerator::Create()
    {
        GeneratorBase::Create();
    }

    void BRDFLUTGenerator::ReCreate()
    {
        GeneratorBase::ReCreate();
    }

    void BRDFLUTGenerator::Update(uint32_t imageIndex)
    {
        GeneratorBase::Update(imageIndex);
    }

    void BRDFLUTGenerator::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        GeneratorBase::Bind(commandBuffer, imageIndex);
    }

    void BRDFLUTGenerator::Cleanup()
    {
        GeneratorBase::Cleanup();
    }

    void BRDFLUTGenerator::Destroy()
    {
        GeneratorBase::Destroy();
    }

    void BRDFLUTGenerator::CreateRenderPass()
    {
        GeneratorBase::CreateRenderPass();
    }

    void BRDFLUTGenerator::CreateFramebuffer()
    {
        GeneratorBase::CreateFramebuffer();
    }

    void BRDFLUTGenerator::CreateDescriptorSets(uint32_t images)
    {
        GeneratorBase::CreateDescriptorSets(images);
    }
}