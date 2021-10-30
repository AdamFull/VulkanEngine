#pragma once

namespace Engine
{
    namespace Core
    {
        class Device;
        namespace Descriptor
        {
            class VulkanDescriptorPool;
            class VulkanDescriptorSetLayout;
            class VulkanDescriptorSet
            {
            public:
                VulkanDescriptorSet() = default;
                void Create(std::unique_ptr<Device> &device, std::shared_ptr<VulkanDescriptorPool> pool, std::unique_ptr<VulkanDescriptorSetLayout> &setLayout, uint32_t images);

                void UpdatePipelineInfo(vk::PipelineBindPoint bindPoint, vk::PipelineLayout layout);

                VulkanDescriptorSet(const VulkanDescriptorSet &) = delete;
                VulkanDescriptorSet &operator=(const VulkanDescriptorSet &) = delete;

                void Update(std::unique_ptr<Device> &device, std::vector<vk::WriteDescriptorSet> &vWrites, uint32_t index);
                void Bind(const vk::CommandBuffer &commandBuffer, uint32_t index, uint32_t set) const;

                void Destroy(std::unique_ptr<Device> &device, std::shared_ptr<VulkanDescriptorPool> pool);
                vk::DescriptorSet &Get(uint32_t index) { return descriptorSets.at(index); }

            private:
                std::vector<vk::DescriptorSet> descriptorSets;
                vk::PipelineBindPoint pipelineBindPoint;
                vk::PipelineLayout pipelineLayout;
            };
        }
    }
}