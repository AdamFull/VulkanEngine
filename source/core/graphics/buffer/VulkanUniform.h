#pragma once

namespace Engine
{
    namespace Core
    {
        struct FBlurData
        {
            alignas(4) float blurScale{0.003};
            alignas(4) float blurStrength{1.0};
            alignas(4) int direction{1};
        };

        struct FPostProcess
        {
            alignas(4) float gamma;
            alignas(4) float exposure;
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

        class CUniformBuffer : public utl::non_copy_movable
        {
        public:
            CUniformBuffer() = default;
            ~CUniformBuffer();

            void create(uint32_t inFlightFrames, size_t uniform_size);
            void reCreate(uint32_t inFlightFrames);
            void cleanup();
            void updateUniformBuffer(uint32_t index, void *ubo);

            // Getters
            std::vector<std::unique_ptr<VulkanBuffer>> &getUniformBuffers();
            std::unique_ptr<VulkanBuffer> &getUniformBuffer(uint32_t index);

        private:
            void createUniformBuffers(uint32_t inFlightFrames);
            size_t iUniformSize{0};

            std::vector<std::unique_ptr<VulkanBuffer>> vBuffers;
        };
    }
}