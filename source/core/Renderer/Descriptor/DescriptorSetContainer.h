#pragma once
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"

namespace Engine
{
    using descriptor_set_conatiner_t = std::unordered_map<uint32_t, std::pair<std::unique_ptr<VulkanDescriptorSetLayout>, std::unique_ptr<VulkanDescriptorSet>>>;

    class VulkanDescriptorSetContainer
    {
    public:
        VulkanDescriptorSetContainer() = default;

        void AttachDescriptorSet(std::unique_ptr<VulkanDescriptorSet>&& pSet, std::unique_ptr<VulkanDescriptorSetLayout>&& pLayout);
        void UpdatePipelineInfo(vk::PipelineBindPoint bindPoint, vk::PipelineLayout layout);

        void Update(uint32_t set, uint32_t index, std::vector<vk::WriteDescriptorSet>& vWrites);
        void Bind(const vk::CommandBuffer& commandBuffer, uint32_t index);

        std::unique_ptr<VulkanDescriptorSet>& GetDescriptorSet(uint32_t set);
        std::unique_ptr<VulkanDescriptorSetLayout>& GetSetLayout(uint32_t set);

        std::vector<vk::DescriptorSetLayout> GetLayouts();
    private:
        descriptor_set_conatiner_t m_SetContainer;
    };
}