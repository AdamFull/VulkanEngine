#include "VulkanMaterial.h"
#include "Resources/Textures/VulkanTexture.h"
#include "VulkanUniform.h"
#include "VulkanDevice.h"
#include "VulkanHighLevel.h"
#include "Pipeline/GraphicsPipelineDiffuse.h"

std::map<vk::ShaderStageFlagBits, std::string> vShList = 
{
    {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/vert.spv"},
    {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/frag.spv"}
};

namespace Engine
{
    void MaterialBase::Create(std::shared_ptr<Texture2D> color)
    {
        m_pColor = color;
        m_pPipeline = std::make_shared<GraphicsPipelineDiffuse>();

        FPipelineCreateInfo createInfo = PipelineConfig::CreateDefaultPipelineConfig(1920, 1080, UDevice->GetSamples());
        m_pPipeline->LoadShader(UDevice, vShList);
        m_pPipeline->Create(createInfo, UDevice, USwapChain);
    }

    void MaterialBase::ReCreate()
    {
        ResourceBase::ReCreate();
        
    }

    void MaterialBase::Update(uint32_t imageIndex)
    {
        ResourceBase::Update(imageIndex);
    }

    void MaterialBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        ResourceBase::Bind(commandBuffer, imageIndex);
        m_pPipeline->Bind(commandBuffer);
        UUniform->Bind(commandBuffer, m_pPipeline->GetPipelineLayout(), m_pPipeline->GetDescriptorSet(imageIndex));
    }

    void MaterialBase::Destroy()
    {
        ResourceBase::Destroy();
        m_pColor->Destroy();
    }

    /****************************************************DiffuseMaterial***************************************************************/
    void MaterialDiffuse::Create(std::shared_ptr<Texture2D> color)
    {
        m_pColor = color;
        m_pPipeline = std::make_shared<GraphicsPipelineDiffuse>();

        FPipelineCreateInfo createInfo = PipelineConfig::CreateDefaultPipelineConfig(1920, 1080, UDevice->GetSamples());
        m_pPipeline->LoadShader(UDevice, vShList);
        m_pPipeline->Create(createInfo, UDevice, USwapChain);
    }

    void MaterialDiffuse::ReCreate()
    {
        MaterialBase::ReCreate();
    }

    void MaterialDiffuse::Update(uint32_t imageIndex)
    {
        MaterialBase::Update(imageIndex);

        auto& uniformBuffer = UUniform->GetUniformBuffer(imageIndex);
        auto& descriptorSet = m_pPipeline->GetDescriptorSet(imageIndex);

        vk::DescriptorBufferInfo bufferInfo{};
        //GetCurrentUniform
        bufferInfo.buffer = uniformBuffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(FUniformData);

        std::array<vk::WriteDescriptorSet, 2> descriptorWrites{};
        descriptorWrites[0].dstSet = descriptorSet;
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].dstSet = descriptorSet;
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &m_pColor->GetDescriptor();

        UDevice->GetLogical()->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    void MaterialDiffuse::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        MaterialBase::Bind(commandBuffer, imageIndex);
    }

    void MaterialDiffuse::Destroy()
    {
        MaterialBase::Destroy();
        m_pAmbient->Destroy();
        m_pSpecular->Destroy();
        m_pNormal->Destroy();
    }
}