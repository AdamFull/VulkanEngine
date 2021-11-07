#include "DescriptorSet.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "Core/VulkanDevice.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;

VulkanDescriptorSet::VulkanDescriptorSet(std::shared_ptr<Device> device) :
m_device(device)
{

}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
    m_device->GetLogical().freeDescriptorSets(m_pool->Get(), descriptorSets);
}

void VulkanDescriptorSet::Create(std::shared_ptr<VulkanDescriptorPool> pool, std::unique_ptr<VulkanDescriptorSetLayout> &setLayout, uint32_t images)
{
    m_pool = pool;
    std::vector<vk::DescriptorSetLayout> vSetLayouts(images, setLayout->Get());
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = m_pool->Get();
    allocInfo.descriptorSetCount = images;
    allocInfo.pSetLayouts = vSetLayouts.data();
    descriptorSets.resize(images);

    if (m_device->GetLogical().allocateDescriptorSets(&allocInfo, descriptorSets.data()) != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to create descriptor set!");
    }
}

void VulkanDescriptorSet::UpdatePipelineInfo(vk::PipelineBindPoint bindPoint, vk::PipelineLayout layout)
{
    pipelineBindPoint = bindPoint;
    pipelineLayout = layout;
}

void VulkanDescriptorSet::Update(std::vector<vk::WriteDescriptorSet> &vWrites, uint32_t index)
{
    for (auto &write : vWrites)
        write.dstSet = descriptorSets.at(index);

    m_device->GetLogical().updateDescriptorSets(static_cast<uint32_t>(vWrites.size()), vWrites.data(), 0, nullptr);
}

void VulkanDescriptorSet::Bind(const vk::CommandBuffer &commandBuffer, uint32_t index, uint32_t set) const
{
    commandBuffer.bindDescriptorSets(pipelineBindPoint, pipelineLayout, set, 1, &descriptorSets.at(index), 0, nullptr);
}