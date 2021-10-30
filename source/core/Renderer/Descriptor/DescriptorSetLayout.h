#pragma once

// Based on: https://github.com/blurrypiano/littleVulkanEngine/tree/master/littleVulkanEngine/tutorial20
namespace Engine
{
    using descriptor_set_layout_bindings_t = std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding>;

    namespace Core
    {
        class Device;
        namespace Descriptor
        {
            class VulkanDescriptorSetLayout
            {
            public:
                VulkanDescriptorSetLayout() = default;
                class Builder
                {
                public:
                    Builder() {}

                    Builder &addBinding(uint32_t binding, vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags, uint32_t count = 1);
                    std::unique_ptr<VulkanDescriptorSetLayout> build(std::unique_ptr<Device> &device) const;

                private:
                    descriptor_set_layout_bindings_t bindings{};
                };

                void Create(std::unique_ptr<Device> &device, descriptor_set_layout_bindings_t bindings);

                VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout &) = delete;
                VulkanDescriptorSetLayout &operator=(const VulkanDescriptorSetLayout &) = delete;

                void Destroy(std::unique_ptr<Device> &device);

                vk::DescriptorSetLayout Get() const { return descriptorSetLayout; }
                descriptor_set_layout_bindings_t &GetBindings() { return m_mBindings; }

            private:
                vk::DescriptorSetLayout descriptorSetLayout;
                descriptor_set_layout_bindings_t m_mBindings;
            };
        }
    }
}