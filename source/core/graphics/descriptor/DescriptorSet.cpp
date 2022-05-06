#include "DescriptorSet.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;

CDescriptorSet::~CDescriptorSet()
{
    CDevice::inst()->getLogical().freeDescriptorSets(descriptorPool, vDescriptorSets);
}

void CDescriptorSet::create(vk::PipelineBindPoint bindPoint, vk::PipelineLayout layout, vk::DescriptorPool pool, vk::DescriptorSetLayout setLayout, uint32_t images)
{
    pipelineBindPoint = bindPoint;
    pipelineLayout = layout;
    descriptorPool = pool;

    std::vector<vk::DescriptorSetLayout> vSetLayouts(images, setLayout);
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = images;
    allocInfo.pSetLayouts = vSetLayouts.data();
    vDescriptorSets.resize(images);

    if (CDevice::inst()->getLogical().allocateDescriptorSets(&allocInfo, vDescriptorSets.data()) != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to create descriptor set!");
    }
}

void CDescriptorSet::create(std::shared_ptr<Pipeline::CPipelineBase> pPipeline, uint32_t images)
{
    create(pPipeline->getBindPoint(), pPipeline->getPipelineLayout(), pPipeline->getDescriptorPool(), pPipeline->getDescriptorSetLayout(), images);
}

void CDescriptorSet::update(std::vector<vk::WriteDescriptorSet> &vWrites, uint32_t index)
{
    for (auto &write : vWrites)
        write.dstSet = vDescriptorSets.at(index);

    CDevice::inst()->getLogical().updateDescriptorSets(static_cast<uint32_t>(vWrites.size()), vWrites.data(), 0, nullptr);
}

void CDescriptorSet::update(vk::WriteDescriptorSet &writes, uint32_t index)
{
    writes.dstSet = vDescriptorSets.at(index);
    CDevice::inst()->getLogical().updateDescriptorSets(1, &writes, 0, nullptr);
}

void CDescriptorSet::bind(const vk::CommandBuffer &commandBuffer, uint32_t index) const
{
    commandBuffer.bindDescriptorSets(pipelineBindPoint, pipelineLayout, 0, 1, &vDescriptorSets.at(index), 0, nullptr);
}