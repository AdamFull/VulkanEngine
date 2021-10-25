#include "DescriptorSet.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "Renderer/VulkanDevice.h"

namespace Engine
{
    void VulkanDescriptorSet::Create(std::unique_ptr<Device>& device, std::shared_ptr<VulkanDescriptorPool> pool, std::unique_ptr<VulkanDescriptorSetLayout>& setLayout, uint32_t images)
    {
        std::vector<vk::DescriptorSetLayout> vSetLayouts(images, setLayout->Get());
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = pool->Get();
        allocInfo.descriptorSetCount = images;
        allocInfo.pSetLayouts = descriptorSets.data();
        descriptorSets.resize(images);

        if (device->GetLogical()->allocateDescriptorSets(&allocInfo, descriptorSets.data()) != vk::Result::eSuccess)
        {
            throw std::runtime_error("failed to create descriptor set!");
        }
    }

    void VulkanDescriptorSet::Destroy(std::unique_ptr<Device>& device, std::shared_ptr<VulkanDescriptorPool> pool)
    {
        device->GetLogical()->freeDescriptorSets(pool->Get(), descriptorSets);
    }
}