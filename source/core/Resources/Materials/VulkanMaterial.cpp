#include "VulkanMaterial.h"
#include "Resources/Textures/VulkanTexture.h"
#include "VulkanUniform.h"
#include "VulkanDevice.h"
#include "Pipeline/GraphicsPipelineDiffuse.h"

namespace Engine
{
    void MaterialBase::Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform)
    {
        ResourceBase::Create(device, swapchain, uniform);
    }

    void MaterialBase::ReCreate(std::unique_ptr<Device>& device)
    {
        ResourceBase::ReCreate(device);
        m_pPipeline->
    }

    void MaterialBase::Bind(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer)
    {
        ResourceBase::Bind(device, commandBuffer);
    }

    void MaterialBase::Destroy(std::unique_ptr<Device>& device)
    {
        ResourceBase::Destroy(device);
        m_pColor->Destroy(device);
    }

    /****************************************************DiffuseMaterial***************************************************************/
    void MaterialDiffuse::Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform)
    {
        MaterialBase::Create(device, swapchain, uniform);
    }

    void MaterialDiffuse::ReCreate(std::unique_ptr<Device>& device)
    {
        MaterialBase::ReCreate(device);
    }

    void MaterialDiffuse::Bind(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer)
    {
        MaterialBase::Bind(device, commandBuffer);

        vk::DescriptorBufferInfo bufferInfo{};
        //GetCurrentUniform
        bufferInfo.buffer = m_pUniform->GetUniformBuffers().at(i);
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(FUniformData);

        std::array<vk::WriteDescriptorSet, 2> descriptorWrites{};
        descriptorWrites[0].dstSet = m_pPipeline->GetDescriptorSets()[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].dstSet = m_pPipeline->GetDescriptorSets()[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &m_pColor->descriptor;

        device->GetLogical()->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    void MaterialDiffuse::Destroy(std::unique_ptr<Device>& device)
    {
        MaterialBase::Destroy(device);
        m_pAmbien->Destroy(device);
        m_pSpecular->Destroy(device);
        m_pNormal->Destroy(device);
    }
}