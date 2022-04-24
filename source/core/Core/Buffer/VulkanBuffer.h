#pragma once

namespace Engine
{
    namespace Core
    {
        class VulkanBuffer : public utl::non_copy_movable
        {
        public:
            VulkanBuffer() = default;
            ~VulkanBuffer();

            void Create(vk::DeviceSize instanceSize, uint32_t instanceCount,
                        vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                        vk::DeviceSize minOffsetAlignment = 1);

            void ReCreate(vk::DeviceSize instanceSize, uint32_t instanceCount,
                          vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                          vk::DeviceSize minOffsetAlignment = 1);

            void Clean();

            vk::DescriptorBufferInfo GetDscriptor(vk::DeviceSize size, vk::DeviceSize offset);
            vk::DescriptorBufferInfo GetDscriptor();

            vk::Result MapMem(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
            void UnmapMem();

            void Write(void *idata, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
            vk::Result Flush(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
            vk::DescriptorBufferInfo DescriptorInfo(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
            vk::Result Invalidate(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

            void WriteToIndex(void *idata, int index);
            vk::Result FlushIndex(int index);
            vk::DescriptorBufferInfo DescriptorInfoForIndex(int index);
            vk::Result InvalidateIndex(int index);

            // Getters
            vk::Buffer GetBuffer() const { return m_buffer; }
            void *GetMappedMemory() const { return m_mapped; }
            uint32_t GetInstanceCount() const { return m_instanceCount; }
            vk::DeviceSize GetInstanceSize() const { return m_instanceSize; }
            vk::DeviceSize GetAlignmentSize() const { return m_instanceSize; }
            vk::BufferUsageFlags GetUsageFlags() const { return m_usageFlags; }
            vk::MemoryPropertyFlags GetMemoryPropertyFlags() const { return m_memoryPropertyFlags; }
            vk::DeviceSize GetBufferSize() const { return m_bufferSize; }

        private:
            static vk::DeviceSize GetAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment);
            
        private:
            void *m_mapped = nullptr;
            vk::Buffer m_buffer = VK_NULL_HANDLE;
            vk::DeviceMemory m_memory = VK_NULL_HANDLE;

            vk::DeviceSize m_bufferSize;
            uint32_t m_instanceCount;
            vk::DeviceSize m_instanceSize;
            vk::DeviceSize m_alignmentSize;
            vk::BufferUsageFlags m_usageFlags;
            vk::MemoryPropertyFlags m_memoryPropertyFlags;
        };
    }
}