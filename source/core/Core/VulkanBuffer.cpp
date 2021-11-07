#include "VulkanBuffer.h"
#include "VulkanDevice.h"

using namespace Engine::Core;

VulkanBuffer::VulkanBuffer(std::shared_ptr<Device> device) : m_device(device)
{

}

VulkanBuffer::~VulkanBuffer()
{
    Clean();
}

void VulkanBuffer::Create(vk::DeviceSize instanceSize, uint32_t instanceCount,
                          vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                          vk::DeviceSize minOffsetAlignment)
{
    data.alignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
    data.bufferSize = data.alignmentSize * instanceCount;
    m_device->CreateOnDeviceBuffer(data.bufferSize, usageFlags, memoryPropertyFlags, data.buffer, data.memory);
}

void VulkanBuffer::ReCreate(vk::DeviceSize instanceSize, uint32_t instanceCount,
                            vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                            vk::DeviceSize minOffsetAlignment)
{
    Create(instanceSize, instanceCount, usageFlags, memoryPropertyFlags, minOffsetAlignment);
}

void VulkanBuffer::Clean()
{
    UnmapMem();
    m_device->Destroy(data.buffer);
    m_device->Destroy(data.memory);
}

vk::DescriptorBufferInfo VulkanBuffer::GetDscriptor(vk::DeviceSize size, vk::DeviceSize offset)
{
    return vk::DescriptorBufferInfo{data.buffer, offset, size};
}

vk::DescriptorBufferInfo VulkanBuffer::GetDscriptor()
{
    return GetDscriptor(data.bufferSize, 0);
}

vk::Result VulkanBuffer::MapMem(vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(m_device && data.buffer && data.memory && "Called map on buffer before create");
    if (size == VK_WHOLE_SIZE)
    {
        return m_device->GetLogical().mapMemory(data.memory, 0, data.bufferSize, vk::MemoryMapFlags{}, &data.mapped);
    }
    return m_device->GetLogical().mapMemory(data.memory, offset, size, vk::MemoryMapFlags{}, &data.mapped);
}

void VulkanBuffer::UnmapMem()
{
    if (data.mapped)
    {
        m_device->GetLogical().unmapMemory(data.memory);
        data.mapped = nullptr;
    }
}

void VulkanBuffer::Write(void *idata, vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(m_device && data.mapped && "Cannot copy to unmapped buffer");

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

vk::Result VulkanBuffer::Flush(vk::DeviceSize size, vk::DeviceSize offset)
{
    vk::MappedMemoryRange mappedRange = {};
    mappedRange.memory = data.memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return m_device->GetLogical().flushMappedMemoryRanges(1, &mappedRange);
}

vk::DescriptorBufferInfo VulkanBuffer::DescriptorInfo(vk::DeviceSize size, vk::DeviceSize offset)
{
    return vk::DescriptorBufferInfo{data.buffer, offset, size};
}

vk::Result VulkanBuffer::Invalidate(vk::DeviceSize size, vk::DeviceSize offset)
{
    vk::MappedMemoryRange mappedRange = {};
    mappedRange.memory = data.memory;
    mappedRange.offset = offset;
    mappedRange.size = size;

    return m_device->GetLogical().invalidateMappedMemoryRanges(1, &mappedRange);
}

void VulkanBuffer::WriteToIndex(void *idata, int index)
{
    Write(idata, data.instanceSize, index * data.alignmentSize);
}

vk::Result VulkanBuffer::FlushIndex(int index)
{
    return Flush(data.alignmentSize, index * data.alignmentSize);
}

vk::DescriptorBufferInfo VulkanBuffer::DescriptorInfoForIndex(int index)
{
    return DescriptorInfo(data.alignmentSize, index * data.alignmentSize);
}

vk::Result VulkanBuffer::InvalidateIndex(int index)
{
    return Invalidate(data.alignmentSize, index * data.alignmentSize);
}

vk::DeviceSize VulkanBuffer::GetAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment)
{
    if (minOffsetAlignment > 0)
    {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}