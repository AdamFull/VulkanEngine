#include "VulkanBuffer.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;

CVulkanBuffer::~CVulkanBuffer()
{
    clean();
}

void CVulkanBuffer::create(vk::DeviceSize instanceSize, uint32_t instanceCount,
                          vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                          vk::DeviceSize minOffsetAlignment)
{
    alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
    bufferSize = alignmentSize * instanceCount;
    UDevice->CreateOnDeviceBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, deviceMemory);
}

void CVulkanBuffer::reCreate(vk::DeviceSize instanceSize, uint32_t instanceCount,
                            vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                            vk::DeviceSize minOffsetAlignment)
{
    create(instanceSize, instanceCount, usageFlags, memoryPropertyFlags, minOffsetAlignment);
}

void CVulkanBuffer::clean()
{
    unmapMem();
    UDevice->Destroy(buffer);
    UDevice->Destroy(deviceMemory);
}

vk::DescriptorBufferInfo CVulkanBuffer::getDscriptor(vk::DeviceSize size, vk::DeviceSize offset)
{
    return vk::DescriptorBufferInfo{buffer, offset, size};
}

vk::DescriptorBufferInfo CVulkanBuffer::getDscriptor()
{
    return getDscriptor(bufferSize, 0);
}

vk::Result CVulkanBuffer::mapMem(vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(UDevice && buffer && deviceMemory && "Called map on buffer before create");
    if (size == VK_WHOLE_SIZE)
    {
        return UDevice->GetLogical().mapMemory(deviceMemory, 0, bufferSize, vk::MemoryMapFlags{}, &mappedMemory);
    }
    return UDevice->GetLogical().mapMemory(deviceMemory, offset, size, vk::MemoryMapFlags{}, &mappedMemory);
}

void CVulkanBuffer::unmapMem()
{
    if (mappedMemory)
    {
        UDevice->GetLogical().unmapMemory(deviceMemory);
        mappedMemory = nullptr;
    }
}

void CVulkanBuffer::write(void *idata, vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(UDevice && mappedMemory && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE)
    {
        memcpy(mappedMemory, idata, bufferSize);
    }
    else
    {
        char *memOffset = (char *)mappedMemory;
        memOffset += offset;
        memcpy(memOffset, idata, size);
    }
}

vk::Result CVulkanBuffer::flush(vk::DeviceSize size, vk::DeviceSize offset)
{
    vk::MappedMemoryRange mappedRange = {};
    mappedRange.memory = deviceMemory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return UDevice->GetLogical().flushMappedMemoryRanges(1, &mappedRange);
}

vk::DescriptorBufferInfo CVulkanBuffer::descriptorInfo(vk::DeviceSize size, vk::DeviceSize offset)
{
    return vk::DescriptorBufferInfo{buffer, offset, size};
}

vk::Result CVulkanBuffer::invalidate(vk::DeviceSize size, vk::DeviceSize offset)
{
    vk::MappedMemoryRange mappedRange = {};
    mappedRange.memory = deviceMemory;
    mappedRange.offset = offset;
    mappedRange.size = size;

    return UDevice->GetLogical().invalidateMappedMemoryRanges(1, &mappedRange);
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