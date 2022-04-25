#include "DescriptorSet.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;

DescriptorSet::~DescriptorSet()
{
    UDevice->GetLogical().freeDescriptorSets(m_descriptorPool, m_vDescriptorSets);
}

void DescriptorSet::Create(std::shared_ptr<Pipeline::PipelineBase> pPipeline, uint32_t images)
{
    m_pipelineBindPoint = pPipeline->GetBindPoint();
    m_pipelineLayout = pPipeline->GetPipelineLayout();
    m_descriptorPool = pPipeline->GetDescriptorPool();

    std::vector<vk::DescriptorSetLayout> vSetLayouts(images, pPipeline->GetDescriptorSetLayout());
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = images;
    allocInfo.pSetLayouts = vSetLayouts.data();
    m_vDescriptorSets.resize(images);

    if (UDevice->GetLogical().allocateDescriptorSets(&allocInfo, m_vDescriptorSets.data()) != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to create descriptor set!");
    }
}

void DescriptorSet::Update(std::vector<vk::WriteDescriptorSet> &vWrites, uint32_t index)
{
    for (auto &write : vWrites)
        write.dstSet = m_vDescriptorSets.at(index);

    UDevice->GetLogical().updateDescriptorSets(static_cast<uint32_t>(vWrites.size()), vWrites.data(), 0, nullptr);
}

void DescriptorSet::Bind(const vk::CommandBuffer &commandBuffer, uint32_t index) const
{
    commandBuffer.bindDescriptorSets(m_pipelineBindPoint, m_pipelineLayout, 0, 1, &m_vDescriptorSets.at(index), 0, nullptr);
}