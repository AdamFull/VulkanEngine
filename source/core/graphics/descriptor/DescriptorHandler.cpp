#include "DescriptorHandler.h"
#include "DescriptorSet.h"
#include "graphics/pipeline/Pipeline.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;

void CDescriptorHandler::create(std::shared_ptr<Pipeline::CPipelineBase> pipeline)
{
    uint32_t images = CDevice::inst()->getFramesInFlight();
    pDescriptorSet = std::make_unique<CDescriptorSet>();
    pDescriptorSet->create(pipeline, images);
    pPipeline = pipeline;
}

void CDescriptorHandler::cleanup()
{
    //Custom rules for delete object
    pDescriptorSet->cleanup();
}

void CDescriptorHandler::update(uint32_t index)
{
    auto& vkDevice = CDevice::inst()->getLogical();
    assert(vkDevice && "Trying to update descriptor sets, but device is invalid.");
    for (auto &write : vWriteDescriptorSets)
        write.dstSet = pDescriptorSet->get(index);
    vkDevice.updateDescriptorSets(static_cast<uint32_t>(vWriteDescriptorSets.size()), vWriteDescriptorSets.data(), 0, nullptr);
}

void CDescriptorHandler::bind(const vk::CommandBuffer &commandBuffer, uint32_t index) const
{
    pDescriptorSet->bind(commandBuffer, index);
}

void CDescriptorHandler::reset()
{
    vWriteDescriptorSets.clear();
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo)
{
    auto uniformBlock = pPipeline->getShader()->getUniformBlock(srUniformName);
    auto descriptorType = uniformBlock->getDescriptorType();

    vk::WriteDescriptorSet write{};
    write.descriptorType = descriptorType;
    write.dstBinding = uniformBlock->getBinding();
    write.pBufferInfo = &bufferInfo;
    write.descriptorCount = 1;
    vWriteDescriptorSets.emplace_back(std::move(write));
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::DescriptorImageInfo& imageInfo)
{
    auto uniform = pPipeline->getShader()->getUniform(srUniformName);
    auto descriptorType = uniform->getDescriptorType();

    vk::WriteDescriptorSet write{};
    write.descriptorType = descriptorType;
    write.dstBinding = uniform->getBinding();
    write.pImageInfo = &imageInfo;
    write.descriptorCount = 1;
    vWriteDescriptorSets.emplace_back(std::move(write));
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::WriteDescriptorSet& writeInfo)
{
    vWriteDescriptorSets.emplace_back(std::move(writeInfo));
}