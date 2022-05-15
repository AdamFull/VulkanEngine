#pragma once
#include <glm/glm.hpp>
#include <util/helpers.hpp>

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

        struct FPointLight
        {
            alignas(16) glm::vec3 position;
            alignas(16) glm::vec3 color;
            alignas(4) float radius;
            alignas(4) float intencity;
        };

        struct FDirectionalLight
        {
            alignas(16) glm::vec3 color;
            alignas(16) glm::vec3 direction;
            alignas(4) float intencity;
        };

        struct FSpotLight
        {
            alignas(16) glm::vec3 position;
            alignas(16) glm::vec3 color;
            alignas(16) glm::vec3 direction;
            alignas(4) float intencity;
            alignas(4) float cutoff;
        };

        struct FPointLightsData
        {
            FPointLight lights[256];
            int count;
        };

        struct FDirectionalLightsData
        {
            FDirectionalLight lights[256];
            int count;
        };

        struct FSpotLightsData
        {
            FSpotLight lights[256];
            int count;
        };

        struct FLightningData
        {
            glm::vec3 viewPos;
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

        
        class CVulkanBuffer;

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
            std::vector<std::unique_ptr<CVulkanBuffer>> &getUniformBuffers();
            std::unique_ptr<CVulkanBuffer> &getUniformBuffer(uint32_t index);

        private:
            void createUniformBuffers(uint32_t inFlightFrames);
            size_t iUniformSize{0};
            bool bIsClean{false};

            std::vector<std::unique_ptr<CVulkanBuffer>> vBuffers;
        };
    }
}