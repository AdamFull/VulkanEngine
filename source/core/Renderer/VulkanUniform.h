#pragma once

namespace Engine
{
    struct FUniformData 
    {
        glm::mat4 model{};
        glm::mat4 view{};
        glm::mat4 projection{};
        glm::mat4 normal{};

        glm::vec4 lightPosition{};
        glm::vec4 viewPosition{};
    };

    struct FLightData
    {
        glm::vec4 viewPosition{};
        glm::vec4 lightPosition{};
        glm::vec4 lightColor{};
    };

    struct FUniformDataUI 
    {
		glm::vec2 scale;
		glm::vec2 translate;
	};

    class Device;
    class VulkanBuffer;

    class UniformBuffer
    {
    public:
        UniformBuffer() = default;
        UniformBuffer(const UniformBuffer&) = delete;
        void operator=(const UniformBuffer&) = delete;
        UniformBuffer(UniformBuffer&&) = delete;
        UniformBuffer& operator=(UniformBuffer&&) = delete;

        void Create(std::unique_ptr<Device>& device, uint32_t inFlightFrames, size_t uniform_size);
        void ReCreate(std::unique_ptr<Device>& device, uint32_t inFlightFrames);
        void Cleanup(std::unique_ptr<Device>& device);
        void UpdateUniformBuffer(std::unique_ptr<Device>& device, uint32_t index, void* ubo);

        // Getters
        std::vector<std::unique_ptr<VulkanBuffer>>& GetUniformBuffers();
        std::unique_ptr<VulkanBuffer>& GetUniformBuffer(uint32_t index);
    private:
        void CreateUniformBuffers(std::unique_ptr<Device>& device, uint32_t inFlightFrames);
        size_t m_iUniformSize{0};

        std::vector<std::unique_ptr<VulkanBuffer>> m_pBuffers;
    };
}