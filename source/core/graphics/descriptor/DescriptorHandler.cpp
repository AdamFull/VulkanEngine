#include "DescriptorHandler.h"
#include "DescriptorSet.h"
#include "graphics/pipeline/Pipeline.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;

DescriptorHandler::~DescriptorHandler()
{

}

void DescriptorHandler::Create(std::shared_ptr<Pipeline::CPipelineBase> pPipeline)
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
    auto uniformBlock = m_pPipeline->getShader()->getUniformBlock(srUniformName);
    auto descriptorType = m_pPipeline->getShader()->getDescriptorType(uniformBlock->getBinding());

    vk::WriteDescriptorSet write{};
    write.descriptorType = descriptorType.value();
    write.dstBinding = uniformBlock->getBinding();
    write.pBufferInfo = &bufferInfo;
    write.descriptorCount = 1;
    m_vWriteDescriptorSets.emplace_back(std::move(write));
}

void DescriptorHandler::Set(const std::string& srUniformName, vk::DescriptorImageInfo& imageInfo)
{
    auto uniform = m_pPipeline->getShader()->getUniform(srUniformName);
    auto descriptorType = m_pPipeline->getShader()->getDescriptorType(uniform->getBinding());

    vk::WriteDescriptorSet write{};
    write.descriptorType = descriptorType.value();
    write.dstBinding = uniform->getBinding();
    write.pImageInfo = &imageInfo;
    write.descriptorCount = 1;
    m_vWriteDescriptorSets.emplace_back(std::move(write));
}

void DescriptorHandler::Set(const std::string& srUniformName, vk::WriteDescriptorSet& writeInfo)
{
    m_vWriteDescriptorSets.emplace_back(std::move(writeInfo));
}