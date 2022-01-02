#include "VulkanBuffer.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;

VulkanBuffer::~VulkanBuffer()
{
    Clean();
}

void VulkanBuffer::Create(vk::DeviceSize instanceSize, uint32_t instanceCount,
                          vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                          vk::DeviceSize minOffsetAlignment)
{
    m_alignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
    m_bufferSize = m_alignmentSize * instanceCount;
    UDevice->CreateOnDeviceBuffer(m_bufferSize, usageFlags, memoryPropertyFlags, m_buffer, m_memory);
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
    UDevice->Destroy(m_buffer);
    UDevice->Destroy(m_memory);
}

vk::DescriptorBufferInfo VulkanBuffer::GetDscriptor(vk::DeviceSize size, vk::DeviceSize offset)
{
    return vk::DescriptorBufferInfo{m_buffer, offset, size};
}

vk::DescriptorBufferInfo VulkanBuffer::GetDscriptor()
{
    return GetDscriptor(m_bufferSize, 0);
}

vk::Result VulkanBuffer::MapMem(vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(UDevice && m_buffer && m_memory && "Called map on buffer before create");
    if (size == VK_WHOLE_SIZE)
    {
        return UDevice->GetLogical().mapMemory(m_memory, 0, m_bufferSize, vk::MemoryMapFlags{}, &m_mapped);
    }
    return UDevice->GetLogical().mapMemory(m_memory, offset, size, vk::MemoryMapFlags{}, &m_mapped);
}

void VulkanBuffer::UnmapMem()
{
    if (m_mapped)
    {
        UDevice->GetLogical().unmapMemory(m_memory);
        m_mapped = nullptr;
    }
}

void VulkanBuffer::Write(void *idata, vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(UDevice && m_mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE)
    {
        memcpy(m_mapped, idata, m_bufferSize);
    }
    else
    {
        char *memOffset = (char *)m_mapped;
        memOffset += offset;
        memcpy(memOffset, idata, size);
    }
}

vk::Result VulkanBuffer::Flush(vk::DeviceSize size, vk::DeviceSize offset)
{
    vk::MappedMemoryRange mappedRange = {};
    mappedRange.memory = m_memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return UDevice->GetLogical().flushMappedMemoryRanges(1, &mappedRange);
}

vk::DescriptorBufferInfo VulkanBuffer::DescriptorInfo(vk::DeviceSize size, vk::DeviceSize offset)
{
    return vk::DescriptorBufferInfo{m_buffer, offset, size};
}

vk::Result VulkanBuffer::Invalidate(vk::DeviceSize size, vk::DeviceSize offset)
{
    vk::MappedMemoryRange mappedRange = {};
    mappedRange.memory = m_memory;
    mappedRange.offset = offset;
    mappedRange.size = size;

    return UDevice->GetLogical().invalidateMappedMemoryRanges(1, &mappedRange);
}

void VulkanBuffer::WriteToIndex(void *idata, int index)
{
    Write(idata, m_instanceSize, index * m_alignmentSize);
}

vk::Result VulkanBuffer::FlushIndex(int index)
{
    return Flush(m_alignmentSize, index * m_alignmentSize);
}

vk::DescriptorBufferInfo VulkanBuffer::DescriptorInfoForIndex(int index)
{
    return DescriptorInfo(m_alignmentSize, index * m_alignmentSize);
}

vk::Result VulkanBuffer::InvalidateIndex(int index)
{
    return Invalidate(m_alignmentSize, index * m_alignmentSize);
}

vk::DeviceSize VulkanBuffer::GetAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment)
{
    if (minOffsetAlignment > 0)
    {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}