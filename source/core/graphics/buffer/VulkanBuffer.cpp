#include "VulkanBuffer.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;

CVulkanBuffer::~CVulkanBuffer()
{
    cleanup();
}

void CVulkanBuffer::create(vk::DeviceSize instanceSize, uint32_t instanceCount,
                          vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                          vk::DeviceSize minOffsetAlignment)
{
    auto logical = CDevice::inst()->getLogical();
    alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
    bufferSize = alignmentSize * instanceCount;

    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usageFlags;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    vk::Result res = CDevice::inst()->create(bufferInfo, &buffer);
    assert(res == vk::Result::eSuccess && "On device buffer was not created");

    vk::MemoryRequirements memRequirements = logical.getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo allocInfo = {};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = CDevice::inst()->findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);

    res = CDevice::inst()->create(allocInfo, &deviceMemory);
    assert(res == vk::Result::eSuccess && "Buffer memory was not allocated");

    logical.bindBufferMemory(buffer, deviceMemory, 0);
}

void CVulkanBuffer::reCreate(vk::DeviceSize instanceSize, uint32_t instanceCount,
                            vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                            vk::DeviceSize minOffsetAlignment)
{
    create(instanceSize, instanceCount, usageFlags, memoryPropertyFlags, minOffsetAlignment);
}

void CVulkanBuffer::cleanup()
{
    if(!bIsClean)
    {
        if(mappedMemory)
            unmapMem();
        if(buffer)
            CDevice::inst()->destroy(&buffer);
        if(deviceMemory)
            CDevice::inst()->destroy(&deviceMemory);
        bIsClean = true;
    }
}

vk::DescriptorBufferInfo CVulkanBuffer::getDescriptor(vk::DeviceSize size, vk::DeviceSize offset)
{
    return vk::DescriptorBufferInfo{buffer, offset, size};
}

vk::DescriptorBufferInfo CVulkanBuffer::getDescriptor()
{
    return getDescriptor(bufferSize, 0);
}

vk::Result CVulkanBuffer::mapMem(vk::DeviceSize size, vk::DeviceSize offset)
{
    auto& vkDevice =  CDevice::inst()->getLogical();
    assert(vkDevice && buffer && deviceMemory && "Called map on buffer before create");
    if (size == VK_WHOLE_SIZE)
    {
        return vkDevice.mapMemory(deviceMemory, 0, bufferSize, vk::MemoryMapFlags{}, &mappedMemory);
    }
    return vkDevice.mapMemory(deviceMemory, offset, size, vk::MemoryMapFlags{}, &mappedMemory);
}

void CVulkanBuffer::unmapMem()
{
    auto& vkDevice =  CDevice::inst()->getLogical();
    assert(vkDevice && "Called unmap buffer but device is invalid");
    if (mappedMemory)
    {
        vkDevice.unmapMemory(deviceMemory);
        mappedMemory = nullptr;
    }
}

bool CVulkanBuffer::compare(void *idata, vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(mappedMemory && "Cannot compare to unmapped buffer");
    if (offset == 0)
    {
        return std::memcmp(mappedMemory, idata, size) != 0;
    }
    else
    {
        char *memOffset = (char *)mappedMemory;
        memOffset += offset;
        return std::memcmp(memOffset, idata, size) != 0;
    }
}

void CVulkanBuffer::write(void *idata, vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(mappedMemory && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE)
    {
        std::memcpy(mappedMemory, idata, bufferSize);
    }
    else
    {
        char *memOffset = (char *)mappedMemory;
        memOffset += offset;
        std::memcpy(memOffset, idata, size);
    }
}

vk::Result CVulkanBuffer::flush(vk::DeviceSize size, vk::DeviceSize offset)
{
    auto& vkDevice =  CDevice::inst()->getLogical();
    assert(vkDevice && "Called flush buffer but device is invalid");
    vk::MappedMemoryRange mappedRange = {};
    mappedRange.memory = deviceMemory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkDevice.flushMappedMemoryRanges(1, &mappedRange);
}

vk::DescriptorBufferInfo CVulkanBuffer::descriptorInfo(vk::DeviceSize size, vk::DeviceSize offset)
{
    return vk::DescriptorBufferInfo{buffer, offset, size};
}

vk::Result CVulkanBuffer::invalidate(vk::DeviceSize size, vk::DeviceSize offset)
{
    auto& vkDevice =  CDevice::inst()->getLogical();
    assert(vkDevice && "Called invalidate buffer but device is invalid");
    vk::MappedMemoryRange mappedRange = {};
    mappedRange.memory = deviceMemory;
    mappedRange.offset = offset;
    mappedRange.size = size;

    return vkDevice.invalidateMappedMemoryRanges(1, &mappedRange);
}

void CVulkanBuffer::writeToIndex(void *idata, int index)
{
    write(idata, instanceSize, index * alignmentSize);
}

vk::Result CVulkanBuffer::flushIndex(int index)
{
    return flush(alignmentSize, index * alignmentSize);
}

vk::DescriptorBufferInfo CVulkanBuffer::descriptorInfoForIndex(int index)
{
    return descriptorInfo(alignmentSize, index * alignmentSize);
}

vk::Result CVulkanBuffer::invalidateIndex(int index)
{
    return invalidate(alignmentSize, index * alignmentSize);
}

vk::DeviceSize CVulkanBuffer::getAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment)
{
    if (minOffsetAlignment > 0)
    {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}