#include "DescriptorSet.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine::core;
using namespace engine::core::pipeline;
using namespace engine::core::descriptor;

void CDescriptorSet::create(vk::PipelineBindPoint bindPoint, vk::PipelineLayout& layout, vk::DescriptorPool& pool, vk::DescriptorSetLayout& setLayout)
{
    pipelineBindPoint = bindPoint;
    pipelineLayout = layout;
    descriptorPool = pool;

    auto framesInFlight = CDevice::inst()->getFramesInFlight();
    std::vector<vk::DescriptorSetLayout> vSetLayouts(framesInFlight, setLayout);
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = framesInFlight;
    allocInfo.pSetLayouts = vSetLayouts.data();
    vDescriptorSets.resize(framesInFlight);

    vk::Result res = CDevice::inst()->create(allocInfo, vDescriptorSets.data());
    assert(res == vk::Result::eSuccess && "Cannot create descriptor sets.");
}

void CDescriptorSet::create(ref_ptr<pipeline::CPipelineBase>& pPipeline)
{
    create(pPipeline->getBindPoint(), pPipeline->getPipelineLayout(), pPipeline->getDescriptorPool(), pPipeline->getDescriptorSetLayout());
}

void CDescriptorSet::cleanup()
{
    //TODO: add destroy method
    auto& vkDevice = CDevice::inst()->getLogical();
    assert(vkDevice && "Trying to free descriptor sets, but device is invalid.");
    vkDevice.freeDescriptorSets(descriptorPool, vDescriptorSets);
}

void CDescriptorSet::update(std::vector<vk::WriteDescriptorSet> &vWrites)
{
    auto& vkDevice = CDevice::inst()->getLogical();
    assert(vkDevice && "Trying to update descriptor sets, but device is invalid.");
    for (auto &write : vWrites)
        write.dstSet = get();

    vkDevice.updateDescriptorSets(static_cast<uint32_t>(vWrites.size()), vWrites.data(), 0, nullptr);
}

void CDescriptorSet::update(vk::WriteDescriptorSet &writes)
{
    auto& vkDevice = CDevice::inst()->getLogical();
    assert(vkDevice && "Trying to free descriptor sets, but device is invalid.");
    writes.dstSet = get();
    vkDevice.updateDescriptorSets(1, &writes, 0, nullptr);
}

void CDescriptorSet::bind(const vk::CommandBuffer &commandBuffer) const
{
    auto currentFrame = CDevice::inst()->getCurrentFrame();
    commandBuffer.bindDescriptorSets(pipelineBindPoint, pipelineLayout, 0, 1, &vDescriptorSets.at(currentFrame), 0, nullptr);
}

vk::DescriptorSet& CDescriptorSet::get()
{
    auto currentFrame = CDevice::inst()->getCurrentFrame();
    return vDescriptorSets.at(currentFrame);
}