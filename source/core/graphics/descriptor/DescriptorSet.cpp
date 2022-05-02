#include "DescriptorSet.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;

CDescriptorSet::~CDescriptorSet()
{
    UDevice->GetLogical().freeDescriptorSets(descriptorPool, vDescriptorSets);
}

void CDescriptorSet::create(std::shared_ptr<Pipeline::CPipelineBase> pPipeline, uint32_t images)
{
    pipelineBindPoint = pPipeline->getBindPoint();
    pipelineLayout = pPipeline->getPipelineLayout();
    descriptorPool = pPipeline->getDescriptorPool();

    std::vector<vk::DescriptorSetLayout> vSetLayouts(images, pPipeline->getDescriptorSetLayout());
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = images;
    allocInfo.pSetLayouts = vSetLayouts.data();
    vDescriptorSets.resize(images);

    if (UDevice->GetLogical().allocateDescriptorSets(&allocInfo, vDescriptorSets.data()) != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to create descriptor set!");
    }
}

void CDescriptorSet::update(std::vector<vk::WriteDescriptorSet> &vWrites, uint32_t index)
{
    for (auto &write : vWrites)
        write.dstSet = vDescriptorSets.at(index);

    UDevice->GetLogical().updateDescriptorSets(static_cast<uint32_t>(vWrites.size()), vWrites.data(), 0, nullptr);
}

void CDescriptorSet::bind(const vk::CommandBuffer &commandBuffer, uint32_t index) const
{
    commandBuffer.bindDescriptorSets(pipelineBindPoint, pipelineLayout, 0, 1, &vDescriptorSets.at(index), 0, nullptr);
}