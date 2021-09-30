#pragma once

namespace Engine
{
    class Device;

    struct FUniformData 
    {
        glm::mat4 transform{1.f};
        glm::mat4 normalMatrix{1.f};
    };

    struct FUniformBuffer
    {
        std::vector<vk::Buffer> vUniformBuffers{};
        std::vector<vk::DeviceMemory> vUniformBuffersMemory{};
    };

    class UniformBuffer
    {
    public:
        void Create(std::unique_ptr<Device>& device, size_t images);
        void ReCreate(std::unique_ptr<Device>& device, size_t images);

        void Cleanup(std::unique_ptr<Device>& device);

        void Bind(vk::CommandBuffer& commandBuffer, vk::PipelineLayout& layout, vk::DescriptorSet descriptorSet);

        void UpdateUniformBuffer(std::unique_ptr<Device>& device, uint32_t index, glm::mat4 transform);

        // Getters
        inline std::vector<vk::Buffer>& GetUniformBuffers() { return data.vUniformBuffers; }
        inline vk::Buffer& GetUniformBuffer(uint32_t index) { return data.vUniformBuffers[index]; }
        inline std::vector<vk::DeviceMemory>& GetUniformBuffersMemory() { return data.vUniformBuffersMemory; }
    private:
        void CreateUniformBuffers(std::unique_ptr<Device>& device, size_t images);

        FUniformBuffer data;
    };
}