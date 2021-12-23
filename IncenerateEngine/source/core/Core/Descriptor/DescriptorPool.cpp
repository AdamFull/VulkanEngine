#include "DescriptorPool.h"
#include "Core/VulkanAllocator.h"
#include "Core/VulkanDevice.h"

using namespace Engine::Core;
using namespace Engine::Core::Descriptor;

VulkanDescriptorPool::VulkanDescriptorPool(std::shared_ptr<Device> device) :
m_device(device)
{

}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    m_device->Destroy(descriptorPool);
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
    auto descriptor_object = FDefaultAllocator::Allocate<VulkanDescriptorPool>();
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
    descriptorPool = m_device->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(descriptorPoolInfo);
}