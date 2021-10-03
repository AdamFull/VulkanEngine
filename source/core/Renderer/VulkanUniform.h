#pragma once

namespace Engine
{
    class Device;
    class VulkanBuffer;

    struct FUniformData 
    {
        glm::mat4 transform{1.f};
        glm::mat4 worldNormal{1.f};
        glm::vec3 lightPosition{0.f, 1.f, 0.f};
    };

    class UniformBuffer
    {
    public:
        UniformBuffer() = default;
        UniformBuffer(const UniformBuffer&) = delete;
        void operator=(const UniformBuffer&) = delete;
        UniformBuffer(UniformBuffer&&) = delete;
        UniformBuffer& operator=(UniformBuffer&&) = delete;

        void Create(std::unique_ptr<Device>& device, uint32_t inFlightFrames);
        void ReCreate(std::unique_ptr<Device>& device, uint32_t inFlightFrames);

        void Cleanup(std::unique_ptr<Device>& device);

        void Bind(vk::CommandBuffer& commandBuffer, vk::PipelineLayout& layout, vk::DescriptorSet descriptorSet);

        void UpdateUniformBuffer(std::unique_ptr<Device>& device, uint32_t index, FUniformData ubo);

        // Getters
        inline std::vector<std::unique_ptr<VulkanBuffer>>& GetUniformBuffers() { return m_pBuffers; }
        inline std::unique_ptr<VulkanBuffer>& GetUniformBuffer(uint32_t index) { return m_pBuffers[index]; }
    private:
        void CreateUniformBuffers(std::unique_ptr<Device>& device, uint32_t inFlightFrames);

        std::vector<std::unique_ptr<VulkanBuffer>> m_pBuffers;
    };
}