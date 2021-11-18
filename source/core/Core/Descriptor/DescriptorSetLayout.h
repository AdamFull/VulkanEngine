#pragma once

// Based on: https://github.com/blurrypiano/littleVulkanEngine/tree/master/littleVulkanEngine/tutorial20
namespace Engine
{
    using descriptor_set_layout_bindings_t = std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding>;

    namespace Core
    {
        
        namespace Descriptor
        {
            class VulkanDescriptorSetLayout
            {
            public:
                VulkanDescriptorSetLayout() = default;
                ~VulkanDescriptorSetLayout();
                class Builder
                {
                public:
                    Builder() {}

                    Builder &addBinding(uint32_t binding, vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags, uint32_t count = 1);
                    std::unique_ptr<VulkanDescriptorSetLayout> build() const;

                private:
                    descriptor_set_layout_bindings_t bindings{};
                };

                void Create(descriptor_set_layout_bindings_t bindings);

                VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout &) = delete;
                VulkanDescriptorSetLayout &operator=(const VulkanDescriptorSetLayout &) = delete;

                void Destroy();

                vk::DescriptorSetLayout Get() const { return descriptorSetLayout; }
                descriptor_set_layout_bindings_t &GetBindings() { return m_mBindings; }

            private:
                vk::DescriptorSetLayout descriptorSetLayout;
                descriptor_set_layout_bindings_t m_mBindings;
            };
        }
    }
}