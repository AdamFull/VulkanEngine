#include "DescriptorSetContainer.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core::Descriptor;

void VulkanDescriptorSetContainer::AttachDescriptorSet(std::unique_ptr<VulkanDescriptorSet> &&pSet, std::unique_ptr<VulkanDescriptorSetLayout> &&pLayout)
{
    uint32_t index = m_SetContainer.size();
    m_SetContainer.emplace(std::move(std::make_pair(index, std::make_pair(std::move(pLayout), std::move(pSet)))));
}

void VulkanDescriptorSetContainer::UpdatePipelineInfo(vk::PipelineBindPoint bindPoint, vk::PipelineLayout layout)
{
    for (auto &[set, pair] : m_SetContainer)
    {
        pair.second->UpdatePipelineInfo(bindPoint, layout);
    }
}

void VulkanDescriptorSetContainer::Update(uint32_t set, uint32_t index, std::vector<vk::WriteDescriptorSet> &vWrites)
{
    m_SetContainer[set].second->Update(vWrites, index);
}

void VulkanDescriptorSetContainer::Bind(const vk::CommandBuffer &commandBuffer, uint32_t index)
{
    for (auto &[set, pair] : m_SetContainer)
    {
        pair.second->Bind(commandBuffer, index, set);
    }
}

std::unique_ptr<VulkanDescriptorSet> &VulkanDescriptorSetContainer::GetDescriptorSet(uint32_t set)
{
    return m_SetContainer[set].second;
}

std::unique_ptr<VulkanDescriptorSetLayout> &VulkanDescriptorSetContainer::GetSetLayout(uint32_t set)
{
    return m_SetContainer[set].first;
}

std::vector<vk::DescriptorSetLayout> VulkanDescriptorSetContainer::GetLayouts()
{
    std::vector<vk::DescriptorSetLayout> vLayouts{};
    for (auto &[set, pair] : m_SetContainer)
        vLayouts.push_back(pair.first->Get());
    return vLayouts;
}