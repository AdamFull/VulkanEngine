#include "VulkanMaterial.h"
#include "VulkanTexture.h"
#include "VulkanUniform.h"
#include "VulkanDevice.h"
#include "core/Pipeline/GraphicsPipelineDiffuse.h"

namespace Engine
{
    void MaterialDiffuse::Bind(vk::CommandBuffer& commandBuffer)
    {
        vk::DescriptorBufferInfo bufferInfo{};
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

    void MaterialDiffuse::Draw(vk::CommandBuffer& commandBuffer)
    {
        
    }
}