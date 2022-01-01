#include "DescriptorHandler.h"
#include "DescriptorSet.h"
#include "Core/Pipeline/VulkanPipeline.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;

DescriptorHandler::~DescriptorHandler()
{

}

void DescriptorHandler::Create(std::shared_ptr<Pipeline::PipelineBase> pPipeline)
{
    uint32_t images = USwapChain->GetImages().size();
    m_pDescriptorSet = std::make_unique<DescriptorSet>();
    m_pDescriptorSet->Create(pPipeline, images);
    m_pPipeline = pPipeline;
}

void DescriptorHandler::Update(uint32_t index)
{
    for (auto &write : m_vWriteDescriptorSets)
        write.dstSet = m_pDescriptorSet->Get(index);
    UDevice->GetLogical().updateDescriptorSets(static_cast<uint32_t>(m_vWriteDescriptorSets.size()), m_vWriteDescriptorSets.data(), 0, nullptr);
}

void DescriptorHandler::Bind(const vk::CommandBuffer &commandBuffer, uint32_t index) const
{
    m_pDescriptorSet->Bind(commandBuffer, index);
}

void DescriptorHandler::Clear()
{
    m_vWriteDescriptorSets.clear();
}

void DescriptorHandler::Set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo)
{
    auto uniformBlock = m_pPipeline->GetShader()->GetUniformBlock(srUniformName);
    auto descriptorType = m_pPipeline->GetShader()->GetDescriptorType(uniformBlock->GetBinding());

    vk::WriteDescriptorSet write{};
    write.descriptorType = descriptorType.value();
    write.dstBinding = uniformBlock->GetBinding();
    write.pBufferInfo = &bufferInfo;
    write.descriptorCount = 1;
    m_vWriteDescriptorSets.emplace_back(std::move(write));
}

void DescriptorHandler::Set(const std::string& srUniformName, vk::DescriptorImageInfo& imageInfo)
{
    auto uniform = m_pPipeline->GetShader()->GetUniform(srUniformName);
    auto descriptorType = m_pPipeline->GetShader()->GetDescriptorType(uniform->GetBinding());

    vk::WriteDescriptorSet write{};
    write.descriptorType = descriptorType.value();
    write.dstBinding = uniform->GetBinding();
    write.pImageInfo = &imageInfo;
    write.descriptorCount = 1;
    m_vWriteDescriptorSets.emplace_back(std::move(write));
}
