#pragma once

namespace Engine
{
    class Device;

    struct FBuffer
    {
        void* mapped = nullptr;
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
        void Create(std::unique_ptr<Device>& device, vk::DeviceSize instanceSize, uint32_t instanceCount,
                    vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                    vk::DeviceSize minOffsetAlignment = 1);

        void ReCreate(std::unique_ptr<Device>& device, vk::DeviceSize instanceSize, uint32_t instanceCount,
                    vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                    vk::DeviceSize minOffsetAlignment = 1);

        void Destroy(std::unique_ptr<Device>& device);

        vk::Result MapMem(std::unique_ptr<Device>& device, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
        void UnmapMem(std::unique_ptr<Device>& device);

        void Write(std::unique_ptr<Device>& device, void* idata, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
        vk::Result Flush(std::unique_ptr<Device>& device, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
        vk::DescriptorBufferInfo DescriptorInfo(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
        vk::Result Invalidate(std::unique_ptr<Device>& device, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

        void WriteToIndex(std::unique_ptr<Device>& device, void* idata, int index);
        vk::Result FlushIndex(std::unique_ptr<Device>& device, int index);
        vk::DescriptorBufferInfo DescriptorInfoForIndex(int index);
        vk::Result InvalidateIndex(std::unique_ptr<Device>& device, int index);

        //Getters
        vk::Buffer GetBuffer() const { return data.buffer; }
        void* GetMappedMemory() const { return data.mapped; }
        uint32_t GetInstanceCount() const { return data.instanceCount; }
        vk::DeviceSize GetInstanceSize() const { return data.instanceSize; }
        vk::DeviceSize GetAlignmentSize() const { return data.instanceSize; }
        vk::BufferUsageFlags GetUsageFlags() const { return data.usageFlags; }
        vk::MemoryPropertyFlags GetMemoryPropertyFlags() const { return data.memoryPropertyFlags; }
        vk::DeviceSize GetBufferSize() const { return data.bufferSize; }
    private:
        static vk::DeviceSize GetAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment);

        FBuffer data;
    };
}