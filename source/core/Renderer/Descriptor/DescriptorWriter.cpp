#include "DescriptorWriter.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "DescriptorSet.h"

#include "Renderer/VulkanDevice.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;

VulkanDescriptorWriter &VulkanDescriptorWriter::WriteBuffer(uint32_t binding, descriptor_set_layout_bindings_t &bindings, vk::DescriptorBufferInfo *bufferInfo)
{
    assert(bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    vk::WriteDescriptorSet write{};
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    vWrites.push_back(write);
    return *this;
}

VulkanDescriptorWriter &VulkanDescriptorWriter::WriteImage(uint32_t binding, descriptor_set_layout_bindings_t &bindings, vk::DescriptorImageInfo *imageInfo)
{
    assert(bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = bindings[binding];

    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    vk::WriteDescriptorSet write{};
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 1;

    vWrites.push_back(write);
    return *this;
}

std::vector<vk::WriteDescriptorSet> VulkanDescriptorWriter::Build()
{
    return vWrites;
}