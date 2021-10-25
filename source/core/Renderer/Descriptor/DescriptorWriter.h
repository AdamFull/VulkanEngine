#pragma once

//Based on: https://github.com/blurrypiano/littleVulkanEngine/tree/master/littleVulkanEngine/tutorial20
namespace Engine
{
    class VulkanDescriptorPool;
    class VulkanDescriptorSetLayout;
    class VulkanDescriptorSet;
    class VulkanDescriptorWriter
    {
    public:
        void Create(std::unique_ptr<VulkanDescriptorSetLayout>&& setLayout, std::shared_ptr<VulkanDescriptorPool> pool, std::unique_ptr<VulkanDescriptorSet>&& set);

        void WriteBuffer(uint32_t binding, vk::DescriptorBufferInfo *bufferInfo);
        void WriteImage(uint32_t binding, vk::DescriptorImageInfo *imageInfo);

        void Update(std::unique_ptr<Device>& device, uint32_t index);
        void Destroy(std::unique_ptr<Device>& device);

        vk::DescriptorSetLayout& GetSetLayout() { return pSetLayout->Get(); }
        vk::DescriptorPool& GetPool() { return pPool->Get(); }
        vk::DescriptorSet& GetSet(uint32_t index) { return pSet->Get(index); }
    private:
        std::unique_ptr<VulkanDescriptorSetLayout> pSetLayout;
        std::shared_ptr<VulkanDescriptorPool> pPool;
        std::unique_ptr<VulkanDescriptorSet> pSet;
        std::vector<vk::WriteDescriptorSet> vWrites;
    };
}