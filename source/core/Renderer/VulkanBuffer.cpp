#include "VulkanBuffer.h"
#include "VulkanDevice.h"

using namespace Engine::Core;

void VulkanBuffer::Create(std::unique_ptr<Device> &device, vk::DeviceSize instanceSize, uint32_t instanceCount,
                          vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                          vk::DeviceSize minOffsetAlignment)
{
    data.alignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
    data.bufferSize = data.alignmentSize * instanceCount;
    device->CreateOnDeviceBuffer(data.bufferSize, usageFlags, memoryPropertyFlags, data.buffer, data.memory);
}

void VulkanBuffer::ReCreate(std::unique_ptr<Device> &device, vk::DeviceSize instanceSize, uint32_t instanceCount,
                            vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                            vk::DeviceSize minOffsetAlignment)
{
    Create(device, instanceSize, instanceCount, usageFlags, memoryPropertyFlags, minOffsetAlignment);
}

void VulkanBuffer::Destroy(std::unique_ptr<Device> &device)
{
    UnmapMem(device);
    device->Destroy(data.buffer);
    device->Destroy(data.memory);
}

vk::DescriptorBufferInfo VulkanBuffer::GetDscriptor(vk::DeviceSize size, vk::DeviceSize offset)
{
    return vk::DescriptorBufferInfo{data.buffer, offset, size};
}

vk::DescriptorBufferInfo VulkanBuffer::GetDscriptor()
{
    return GetDscriptor(data.bufferSize, 0);
}

vk::Result VulkanBuffer::MapMem(std::unique_ptr<Device> &device, vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(device && data.buffer && data.memory && "Called map on buffer before create");
    if (size == VK_WHOLE_SIZE)
    {
        return device->GetLogical()->mapMemory(data.memory, 0, data.bufferSize, vk::MemoryMapFlags{}, &data.mapped);
    }
    return device->GetLogical()->mapMemory(data.memory, offset, size, vk::MemoryMapFlags{}, &data.mapped);
}

void VulkanBuffer::UnmapMem(std::unique_ptr<Device> &device)
{
    if (data.mapped)
    {
        device->GetLogical()->unmapMemory(data.memory);
        data.mapped = nullptr;
    }
}

void VulkanBuffer::Write(std::unique_ptr<Device> &device, void *idata, vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(device && data.mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE)
    {
        memcpy(data.mapped, idata, data.bufferSize);
    }
    else
    {
        char *memOffset = (char *)data.mapped;
        memOffset += offset;
        memcpy(memOffset, idata, size);
    }
}

vk::Result VulkanBuffer::Flush(std::unique_ptr<Device> &device, vk::DeviceSize size, vk::DeviceSize offset)
{
    vk::MappedMemoryRange mappedRange = {};
    mappedRange.memory = data.memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return device->GetLogical()->flushMappedMemoryRanges(1, &mappedRange);
}

vk::DescriptorBufferInfo VulkanBuffer::DescriptorInfo(vk::DeviceSize size, vk::DeviceSize offset)
{
    return vk::DescriptorBufferInfo{data.buffer, offset, size};
}

vk::Result VulkanBuffer::Invalidate(std::unique_ptr<Device> &device, vk::DeviceSize size, vk::DeviceSize offset)
{
    vk::MappedMemoryRange mappedRange = {};
    mappedRange.memory = data.memory;
    mappedRange.offset = offset;
    mappedRange.size = size;

    return device->GetLogical()->invalidateMappedMemoryRanges(1, &mappedRange);
}

void VulkanBuffer::WriteToIndex(std::unique_ptr<Device> &device, void *idata, int index)
{
    Write(device, idata, data.instanceSize, index * data.alignmentSize);
}

vk::Result VulkanBuffer::FlushIndex(std::unique_ptr<Device> &device, int index)
{
    return Flush(device, data.alignmentSize, index * data.alignmentSize);
}

vk::DescriptorBufferInfo VulkanBuffer::DescriptorInfoForIndex(int index)
{
    return DescriptorInfo(data.alignmentSize, index * data.alignmentSize);
}

vk::Result VulkanBuffer::InvalidateIndex(std::unique_ptr<Device> &device, int index)
{
    return Invalidate(device, data.alignmentSize, index * data.alignmentSize);
}

vk::DeviceSize VulkanBuffer::GetAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment)
{
    if (minOffsetAlignment > 0)
    {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}