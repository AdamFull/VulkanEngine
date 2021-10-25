#pragma once

namespace Engine
{
    class Device;
    class VulkanDescriptorPool;
    class VulkanDescriptorSetLayout;
    class VulkanDescriptorSet
    {
    public:
        VulkanDescriptorSet() = default;
        void Create(std::unique_ptr<Device>& device, std::shared_ptr<VulkanDescriptorPool> pool, std::unique_ptr<VulkanDescriptorSetLayout>& setLayout, uint32_t images);

        VulkanDescriptorSet(const VulkanDescriptorSet &) = delete;
        VulkanDescriptorSet &operator=(const VulkanDescriptorSet &) = delete;

        void Destroy(std::unique_ptr<Device>& device, std::shared_ptr<VulkanDescriptorPool> pool);
        vk::DescriptorSet& Get(uint32_t index) { return descriptorSets.at(index); }

    private:
        std::vector<vk::DescriptorSet> descriptorSets;
    };
}