#pragma once
#include "DescriptorSetLayout.h"

//Based on: https://github.com/blurrypiano/littleVulkanEngine/tree/master/littleVulkanEngine/tutorial20
namespace Engine
{
    class Device;
    class VulkanDescriptorWriter
    {
    public:
        VulkanDescriptorWriter() = default;

        VulkanDescriptorWriter& WriteBuffer(uint32_t binding, descriptor_set_layout_bindings_t& bindings, vk::DescriptorBufferInfo *bufferInfo);
        VulkanDescriptorWriter& WriteImage(uint32_t binding, descriptor_set_layout_bindings_t& bindings, vk::DescriptorImageInfo *imageInfo);

        std::vector<vk::WriteDescriptorSet> Build();
    private:
        std::vector<vk::WriteDescriptorSet> vWrites;
    };
}