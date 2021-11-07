#pragma once

// Based on: https://github.com/blurrypiano/littleVulkanEngine/tree/master/littleVulkanEngine/tutorial20
namespace Engine
{
    namespace Core
    {
        class Device;
        namespace Descriptor
        {
            class VulkanDescriptorPool
            {
            public:
                VulkanDescriptorPool() = default;
                VulkanDescriptorPool(std::shared_ptr<Device> device);
                ~VulkanDescriptorPool();

                class Builder
                {
                public:
                    Builder() {}

                    Builder &addPoolSize(vk::DescriptorType descriptorType, uint32_t count);
                    Builder &setPoolFlags(vk::DescriptorPoolCreateFlags flags);
                    Builder &setMaxSets(uint32_t count);
                    std::unique_ptr<VulkanDescriptorPool> build() const;

                private:
                    std::vector<vk::DescriptorPoolSize> poolSizes{};
                    uint32_t maxSets = 1000;
                    vk::DescriptorPoolCreateFlags poolFlags{};
                };

                void Create(uint32_t maxSets, vk::DescriptorPoolCreateFlags poolFlags, const std::vector<vk::DescriptorPoolSize> &poolSizes);

                VulkanDescriptorPool(const VulkanDescriptorPool &) = delete;
                VulkanDescriptorPool &operator=(const VulkanDescriptorPool &) = delete;

                vk::DescriptorPool &Get() { return descriptorPool; }

            private:
                vk::DescriptorPool descriptorPool;
                std::shared_ptr<Device> m_device;
            };
        }
    }
}