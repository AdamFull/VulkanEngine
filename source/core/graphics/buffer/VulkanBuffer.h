#pragma once
#include <util/helpers.hpp>

namespace engine
{
    namespace core
    {
        class CVulkanBuffer : public utl::non_copy_movable
        {
        public:
            CVulkanBuffer() = default;
            ~CVulkanBuffer();

            void create(vk::DeviceSize instanceSize, uint32_t instanceCount,
                        vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                        vk::DeviceSize minOffsetAlignment = 1);

            void cleanup();

            void reCreate(vk::DeviceSize instanceSize, uint32_t instanceCount,
                          vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                          vk::DeviceSize minOffsetAlignment = 1);

            vk::DescriptorBufferInfo getDescriptor(vk::DeviceSize size, vk::DeviceSize offset);
            vk::DescriptorBufferInfo getDescriptor();

            vk::Result mapMem(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
            void unmapMem();

            bool compare(void *idata, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
            void write(void *idata, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
            vk::Result flush(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
            vk::DescriptorBufferInfo descriptorInfo(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
            vk::Result invalidate(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

            void writeToIndex(void *idata, int index);
            vk::Result flushIndex(int index);
            vk::DescriptorBufferInfo descriptorInfoForIndex(int index);
            vk::Result invalidateIndex(int index);

            // Getters
            vk::Buffer getBuffer() const { return buffer; }
            void* getMappedMemory() const { return mappedMemory; }
            uint32_t getInstanceCount() const { return instanceCount; }
            vk::DeviceSize getInstanceSize() const { return instanceSize; }
            vk::DeviceSize getAlignmentSize() const { return instanceSize; }
            vk::BufferUsageFlags getUsageFlags() const { return usageFlags; }
            vk::MemoryPropertyFlags getMemoryPropertyFlags() const { return memoryPropertyFlags; }
            vk::DeviceSize getBufferSize() const { return bufferSize; }

        private:
            static vk::DeviceSize getAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment);
            
        private:
            void *mappedMemory = nullptr;
            vk::Buffer buffer = VK_NULL_HANDLE;
            vk::DeviceMemory deviceMemory = VK_NULL_HANDLE;
            bool bIsClean{false};

            vk::DeviceSize bufferSize;
            uint32_t instanceCount;
            vk::DeviceSize instanceSize;
            vk::DeviceSize alignmentSize;
            vk::BufferUsageFlags usageFlags;
            vk::MemoryPropertyFlags memoryPropertyFlags;
        };
    }
}