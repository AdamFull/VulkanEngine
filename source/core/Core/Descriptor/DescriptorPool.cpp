#include "DescriptorPool.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    UDevice->Destroy(descriptorPool);
}

VulkanDescriptorPool::Builder &VulkanDescriptorPool::Builder::addPoolSize(vk::DescriptorType descriptorType, uint32_t count)
{
    poolSizes.push_back({descriptorType, count});
    return *this;
}

VulkanDescriptorPool::Builder &VulkanDescriptorPool::Builder::setPoolFlags(vk::DescriptorPoolCreateFlags flags)
{
    poolFlags = flags;
    return *this;
}
VulkanDescriptorPool::Builder &VulkanDescriptorPool::Builder::setMaxSets(uint32_t count)
{
    maxSets = count;
    return *this;
}

std::unique_ptr<VulkanDescriptorPool> VulkanDescriptorPool::Builder::build() const
{
    auto descriptor_object = std::make_unique<VulkanDescriptorPool>();
    descriptor_object->Create(maxSets, poolFlags, poolSizes);
    return descriptor_object;
}

void VulkanDescriptorPool::Create(uint32_t maxSets, vk::DescriptorPoolCreateFlags poolFlags, const std::vector<vk::DescriptorPoolSize> &poolSizes)
{
    vk::DescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;
    descriptorPool = UDevice->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(descriptorPoolInfo);
}