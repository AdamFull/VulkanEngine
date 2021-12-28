#pragma once

namespace Engine
{
    namespace Core
    {
        struct FPostProcess
        {
            alignas(4) float gamma;
            alignas(4) float exposure;
            alignas(4) float blurScale{0.003};
            alignas(4) float blurStrength{1.0};
            alignas(4) int direction{1};
        };

        struct FLight 
        {
            alignas(16) glm::vec3 position;
            alignas(16) glm::vec3 color;
            alignas(4) float radius;
        };

        struct FLightningData
        {
            glm::vec3 viewPos;
            FLight lights[1024];
            int lightCount;
            float bloomThreshold;
        };

        struct FUniformData
        {
            glm::mat4 model{};
            glm::mat4 view{};
            glm::mat4 projection{};
            glm::mat4 normal{};
            glm::vec4 instancePos[256];
            //float repeat;
        };

        struct FUniformDataUI
        {
            alignas(8) glm::vec2 scale;
            alignas(8) glm::vec2 translate;
        };

        
        class VulkanBuffer;

        class UniformBuffer
        {
        public:
            UniformBuffer() = default;
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

            std::vector<std::unique_ptr<VulkanBuffer>> m_pBuffers;
        };
    }
}