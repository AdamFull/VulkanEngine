#include "DescriptorSet.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine::core;
using namespace engine::core::pipeline;
using namespace engine::core::descriptor;

CDescriptorSet::CDescriptorSet(vk::PipelineBindPoint bindPoint, vk::PipelineLayout& layout, vk::DescriptorPool& pool, vk::DescriptorSetLayout& setLayout)
{
    create(bindPoint, layout, pool, setLayout);
}

CDescriptorSet::CDescriptorSet(utl::scope_ptr<pipeline::CPipelineBase>& pPipeline)
{
    create(pPipeline->getBindPoint(), pPipeline->getPipelineLayout(), pPipeline->getDescriptorPool(), pPipeline->getDescriptorSetLayout());
}

CDescriptorSet::~CDescriptorSet()
{
    auto& vkDevice = UDevice->getLogical();
    if(!descriptorPool)
        vkDevice.freeDescriptorSets(descriptorPool, vDescriptorSets);
    vDescriptorSets.clear();
}

void CDescriptorSet::create(vk::PipelineBindPoint bindPoint, vk::PipelineLayout& layout, vk::DescriptorPool& pool, vk::DescriptorSetLayout& setLayout)
{
    pipelineBindPoint = bindPoint;
    pipelineLayout = layout;
    descriptorPool = pool;

    auto framesInFlight = UDevice->getFramesInFlight();
    std::vector<vk::DescriptorSetLayout> vSetLayouts(framesInFlight, setLayout);
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = framesInFlight;
    allocInfo.pSetLayouts = vSetLayouts.data();
    vDescriptorSets.resize(framesInFlight);

    vk::Result res = UDevice->create(allocInfo, vDescriptorSets.data());
    assert(res == vk::Result::eSuccess && "Cannot create descriptor sets.");
}

void CDescriptorSet::update(std::vector<vk::WriteDescriptorSet> &vWrites)
{
    auto& vkDevice = UDevice->getLogical();
    assert(vkDevice && "Trying to update descriptor sets, but device is invalid.");
    for (auto &write : vWrites)
        write.dstSet = get();

    vkDevice.updateDescriptorSets(static_cast<uint32_t>(vWrites.size()), vWrites.data(), 0, nullptr);
}

void CDescriptorSet::update(vk::WriteDescriptorSet &writes)
{
    auto& vkDevice = UDevice->getLogical();
    assert(vkDevice && "Trying to free descriptor sets, but device is invalid.");
    writes.dstSet = get();
    vkDevice.updateDescriptorSets(1, &writes, 0, nullptr);
}

void CDescriptorSet::bind(const vk::CommandBuffer &commandBuffer) const
{
    auto currentFrame = UDevice->getCurrentFrame();
    commandBuffer.bindDescriptorSets(pipelineBindPoint, pipelineLayout, 0, 1, &vDescriptorSets.at(currentFrame), 0, nullptr);
}

vk::DescriptorSet& CDescriptorSet::get()
{
    auto currentFrame = UDevice->getCurrentFrame();
    return vDescriptorSets.at(currentFrame);
}