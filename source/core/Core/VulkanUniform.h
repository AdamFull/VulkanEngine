#pragma once

namespace Engine
{
    namespace Core
    {
        struct Light 
        {
            alignas(16) glm::vec4 position;
            alignas(16) glm::vec3 color;
            alignas(4) float radius;
        };

        struct FLightningData
        {
            alignas(16) glm::vec4 viewPos;
            alignas(16) Light lights[256];
            alignas(4) int lightCount;
            alignas(4) float ambient;
            alignas(4) float tone;
            alignas(4) float gamma;
        };

        struct FUniformData
        {
            alignas(16) glm::mat4 model{};
            alignas(16) glm::mat4 view{};
            alignas(16) glm::mat4 projection{};
            alignas(16) glm::mat4 normal{};
        };

        struct FUniformDataUI
        {
            alignas(8) glm::vec2 scale;
            alignas(8) glm::vec2 translate;
        };

        class Device;
        class VulkanBuffer;

        class UniformBuffer
        {
        public:
            UniformBuffer() = default;
            UniformBuffer(std::shared_ptr<Device> device);
            ~UniformBuffer();

            UniformBuffer(const UniformBuffer &) = delete;
            void operator=(const UniformBuffer &) = delete;
            UniformBuffer(UniformBuffer &&) = delete;
            UniformBuffer &operator=(UniformBuffer &&) = delete;

            void Create(uint32_t inFlightFrames, size_t uniform_size);
            void ReCreate(uint32_t inFlightFrames);
            void Cleanup();
            void UpdateUniformBuffer(uint32_t index, void *ubo);

            // Getters
            std::vector<std::unique_ptr<VulkanBuffer>> &GetUniformBuffers();
            std::unique_ptr<VulkanBuffer> &GetUniformBuffer(uint32_t index);

        private:
            void CreateUniformBuffers(uint32_t inFlightFrames);
            size_t m_iUniformSize{0};

            std::shared_ptr<Device> m_device;
            std::vector<std::unique_ptr<VulkanBuffer>> m_pBuffers;
        };
    }
}