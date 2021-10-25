#include "DescriptorWriter.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "DescriptorSet.h"

#include "Renderer/VulkanDevice.h"

namespace Engine
{
    void VulkanDescriptorWriter::Create(std::unique_ptr<VulkanDescriptorSetLayout>&& setLayout, std::shared_ptr<VulkanDescriptorPool> pool, std::unique_ptr<VulkanDescriptorSet>&& set)
    {
        pSetLayout = std::move(setLayout);
        pPool = pool;
        pSet = std::move(set);
    }

    void VulkanDescriptorWriter::WriteBuffer(uint32_t binding, vk::DescriptorBufferInfo *bufferInfo)
    {
        auto& layoutBindings = pSetLayout->GetBindings();
        assert(layoutBindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto &bindingDescription = layoutBindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        vk::WriteDescriptorSet write{};
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        vWrites.push_back(write);
    }

    void VulkanDescriptorWriter::WriteImage(uint32_t binding, vk::DescriptorImageInfo *imageInfo)
    {
        auto& layoutBindings = pSetLayout->GetBindings();
        assert(layoutBindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto &bindingDescription = layoutBindings[binding];

        assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

        vk::WriteDescriptorSet write{};
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        vWrites.push_back(write);
    }

    void VulkanDescriptorWriter::Update(std::unique_ptr<Device>& device, uint32_t index)
    {
        for (auto &write : vWrites)
        {
            write.dstSet = pSet->Get(index);
        }

        device->GetLogical()->updateDescriptorSets(static_cast<uint32_t>(vWrites.size()), vWrites.data(), 0, nullptr);
    }

    void VulkanDescriptorWriter::Destroy(std::unique_ptr<Device>& device)
    {
        pSet->Destroy(device, pPool);
        pSetLayout->Destroy(device);
    }
}