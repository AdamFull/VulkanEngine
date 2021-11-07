#pragma once

namespace Engine
{
    namespace Core
    {
        class Device;

        struct FBuffer
        {
            void *mapped = nullptr;
            vk::Buffer buffer = VK_NULL_HANDLE;
            vk::DeviceMemory memory = VK_NULL_HANDLE;

            vk::DeviceSize bufferSize;
            uint32_t instanceCount;
            vk::DeviceSize instanceSize;
            vk::DeviceSize alignmentSize;
            vk::BufferUsageFlags usageFlags;
            vk::MemoryPropertyFlags memoryPropertyFlags;
        };

        class VulkanBuffer
        {
        public:
            VulkanBuffer() = default;
            VulkanBuffer(std::shared_ptr<Device> device);
            ~VulkanBuffer();

            VulkanBuffer(const VulkanBuffer &) = delete;
            void operator=(const VulkanBuffer &) = delete;
            VulkanBuffer(VulkanBuffer &&) = delete;
            VulkanBuffer &operator=(VulkanBuffer &&) = delete;

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
            vk::Buffer GetBuffer() const { return data.buffer; }
            void *GetMappedMemory() const { return data.mapped; }
            uint32_t GetInstanceCount() const { return data.instanceCount; }
            vk::DeviceSize GetInstanceSize() const { return data.instanceSize; }
            vk::DeviceSize GetAlignmentSize() const { return data.instanceSize; }
            vk::BufferUsageFlags GetUsageFlags() const { return data.usageFlags; }
            vk::MemoryPropertyFlags GetMemoryPropertyFlags() const { return data.memoryPropertyFlags; }
            vk::DeviceSize GetBufferSize() const { return data.bufferSize; }

        private:
            static vk::DeviceSize GetAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment);
            std::shared_ptr<Device> m_device;

            FBuffer data;
        };
    }
}