#pragma once
#include "graphics/image/Image.h"

#include "graphics/descriptor/DescriptorHandler.h"
#include "graphics/buffer/UniformHandler.hpp"
#include "graphics/buffer/PushHandler.hpp"

#include "resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Core
    {
        class Image;
    }
    namespace Resources
    {
        namespace Material
        {
            struct FMaterialParams
            {
                enum class EAlphaMode
                {
                    EOPAQUE,
                    EMASK,
                    EBLEND
                };

                EAlphaMode alphaMode = EAlphaMode::EOPAQUE;
                float alphaCutoff = 1.0f;
                float metallicFactor = 1.0f;
                float roughnessFactor = 1.0f;
                glm::vec4 baseColorFactor = glm::vec4(1.0f);
            };

            /**
             * @brief Base material representation class
             * 
             */
            class CMaterialBase 
            {
            public:
                friend class CMaterialLoader;
                CMaterialBase() = default;
                ~CMaterialBase();

                /**
                 * @brief Create attached pipelines and descriptors
                 * 
                 */
                virtual void create();

                /**
                 * @brief Add texture descriptor
                 * 
                 * @param attachment Texture attachment name
                 * @param descriptor Texture descriptor
                 */
                void addTexture(const std::string& attachment, vk::DescriptorImageInfo& descriptor);

                /**
                 * @brief Add texture descriptor 
                 * 
                 * @param attachment Texture attachment name
                 * @param pTexture Texture smart object pointer
                 */
                void addTexture(const std::string& attachment, std::shared_ptr<Core::CImage> pTexture);

                /**
                 * @brief Add buffer descriptor
                 * 
                 * @param attachment 
                 * @param descriptor 
                 */
                void addBuffer(const std::string& attachment, vk::DescriptorBufferInfo& descriptor);

                /**
                 * @brief Get texture descriptor
                 * 
                 * @param attachment Texture attachment name
                 * @return vk::DescriptorImageInfo& Descriptor info reference
                 */
                vk::DescriptorImageInfo& getTexture(const std::string& attachment);

                /**
                 * @brief Recreates pipelines
                 * 
                 */
                virtual void reCreate();

                /**
                 * @brief Update material descriptor sets
                 * 
                 * @param imageIndex Swap chain image index
                 */
                virtual void update(uint32_t imageIndex);

                /**
                 * @brief Bind material pipeline and descriptor sets`
                 * 
                 * @param commandBuffer Current command buffer
                 * @param imageIndex Swap chain image index
                 */
                virtual void bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

                /**
                 * @brief Cleanup swapchain and descriptors
                 * 
                 */
                virtual void cleanup();

                /**
                 * @brief Set material additional params
                 * 
                 * @param params New params
                 */
                inline void setParams(FMaterialParams &&params) { m_fMatParams = params; }

                /**
                 * @brief Set material name
                 * 
                 * @param srName New name
                 */
                void setName(const std::string& srName);

                /**
                 * @brief Get material name
                 * 
                 * @return std::string Material name
                 */
                inline std::string getName() { return m_srName; }

                /**
                 * @brief Get material pipeline object
                 * 
                 * @return std::shared_ptr<Core::Pipeline::CPipelineBase> Pipeline smart pointer object
                 */
                inline std::shared_ptr<Core::Pipeline::CPipelineBase> getPipeline() { return m_pPipeline; }

                std::shared_ptr<Core::CUniformHandler>& getUniformBuffer(const std::string& name) { return mUniformBuffers[name]; }

            protected:
                FMaterialParams m_fMatParams{};
                std::string m_srName;

                std::unique_ptr<Core::Descriptor::CDescriptorHandler> m_pDescriptorSet;
                std::map<std::string, std::shared_ptr<Core::CUniformHandler>> mUniformBuffers;
                std::vector<std::shared_ptr<Core::CPushHandler>> m_vPushConstants;
                std::shared_ptr<Core::Pipeline::CPipelineBase> m_pPipeline;
                std::map<std::string, vk::DescriptorImageInfo> m_mTextures;
                std::map<std::string, vk::DescriptorBufferInfo> m_mBuffers;
            };
        }
    }

}