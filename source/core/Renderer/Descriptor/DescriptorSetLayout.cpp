#include "DescriptorSetLayout.h"
#include "Renderer/VulkanDevice.h"

namespace Engine
{
    VulkanDescriptorSetLayout::Builder &VulkanDescriptorSetLayout::Builder::addBinding(uint32_t binding, vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags, uint32_t count)
    {
        assert(bindings.count(binding) == 0 && "Binding already in use");
        vk::DescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<VulkanDescriptorSetLayout> VulkanDescriptorSetLayout::Builder::build(std::unique_ptr<Device>& device) const
    {
        auto descriptor_set_layout_object = std::make_unique<VulkanDescriptorSetLayout>();
        descriptor_set_layout_object->Create(device, bindings);
        return descriptor_set_layout_object;
    }

    void VulkanDescriptorSetLayout::Create(std::unique_ptr<Device>& device, descriptor_set_layout_bindings_t bindings)
    {
        m_mBindings = bindings;
        std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : m_mBindings)
        {
            setLayoutBindings.push_back(kv.second);
        }

        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (device->GetLogical()->createDescriptorSetLayout(&descriptorSetLayoutInfo, nullptr, &descriptorSetLayout) != vk::Result::eSuccess)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void VulkanDescriptorSetLayout::Destroy(std::unique_ptr<Device>& device)
    {
        device->Destroy(descriptorSetLayout);
    }
}