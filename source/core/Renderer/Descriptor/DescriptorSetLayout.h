#pragma once

//Based on: https://github.com/blurrypiano/littleVulkanEngine/tree/master/littleVulkanEngine/tutorial20
namespace Engine
{
    class Device;
    class VulkanDescriptorSetLayout
    {
    public:
        VulkanDescriptorSetLayout() = default;
        class Builder
        {
        public:
            Builder() {}

            Builder &addBinding(uint32_t binding, vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags, uint32_t count = 1);
            std::unique_ptr<VulkanDescriptorSetLayout> build(std::unique_ptr<Device>& device) const;

        private:
            std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings{};
        };

        void Create(std::unique_ptr<Device>& device, std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings);

        VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout &) = delete;
        VulkanDescriptorSetLayout &operator=(const VulkanDescriptorSetLayout &) = delete;

        void Destroy(std::unique_ptr<Device>& device);

        vk::DescriptorSetLayout Get() const { return descriptorSetLayout; }
        std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding>& GetBindings() { return m_mBindings; }

    private:
        vk::DescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> m_mBindings;
    };
}