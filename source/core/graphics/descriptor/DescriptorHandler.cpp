#include "DescriptorHandler.h"
#include "DescriptorSet.h"
#include "graphics/pipeline/Pipeline.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;

CDescriptorHandler::~CDescriptorHandler()
{

}

void CDescriptorHandler::create(std::shared_ptr<Pipeline::CPipelineBase> pipeline)
{
    uint32_t images = USwapChain->getFramesInFlight();
    pDescriptorSet = std::make_unique<CDescriptorSet>();
    pDescriptorSet->create(pipeline, images);
    pPipeline = pipeline;
}

void CDescriptorHandler::update(uint32_t index)
{
    for (auto &write : vWriteDescriptorSets)
        write.dstSet = pDescriptorSet->get(index);
    UDevice->getLogical().updateDescriptorSets(static_cast<uint32_t>(vWriteDescriptorSets.size()), vWriteDescriptorSets.data(), 0, nullptr);
}

void CDescriptorHandler::bind(const vk::CommandBuffer &commandBuffer, uint32_t index) const
{
    pDescriptorSet->bind(commandBuffer, index);
}

void CDescriptorHandler::clear()
{
    vWriteDescriptorSets.clear();
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo)
{
    auto uniformBlock = pPipeline->getShader()->getUniformBlock(srUniformName);
    auto descriptorType = pPipeline->getShader()->getDescriptorType(uniformBlock->getBinding());

    vk::WriteDescriptorSet write{};
    write.descriptorType = descriptorType.value();
    write.dstBinding = uniformBlock->getBinding();
    write.pBufferInfo = &bufferInfo;
    write.descriptorCount = 1;
    vWriteDescriptorSets.emplace_back(std::move(write));
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::DescriptorImageInfo& imageInfo)
{
    auto uniform = pPipeline->getShader()->getUniform(srUniformName);
    auto descriptorType = pPipeline->getShader()->getDescriptorType(uniform->getBinding());

    vk::WriteDescriptorSet write{};
    write.descriptorType = descriptorType.value();
    write.dstBinding = uniform->getBinding();
    write.pImageInfo = &imageInfo;
    write.descriptorCount = 1;
    vWriteDescriptorSets.emplace_back(std::move(write));
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::WriteDescriptorSet& writeInfo)
{
    vWriteDescriptorSets.emplace_back(std::move(writeInfo));
}