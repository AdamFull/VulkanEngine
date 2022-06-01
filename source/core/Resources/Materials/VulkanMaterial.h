#pragma once
#include "graphics/image/Image.h"

#include "graphics/descriptor/DescriptorHandler.h"
#include "graphics/buffer/Handler.h"
#include "graphics/buffer/PushHandler.hpp"

#include "resources/ResourceCunstruct.h"

namespace engine
{
    namespace core { class Image; }
    namespace resources
    {
        namespace material
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
                void addTexture(const std::string& attachment, ref_ptr<core::CImage>& pTexture);

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
                 */
                virtual void update();

                /**
                 * @brief Bind material pipeline and descriptor sets`
                 * 
                 */
                virtual void bind();

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
                 * @return ref_ptr<Core::Pipeline::CPipelineBase> Pipeline smart pointer object
                 */
                inline ref_ptr<core::pipeline::CPipelineBase> getPipeline() { return m_pPipeline; }

                ref_ptr<core::CHandler>& getUniformBuffer(const std::string& name) { return mBuffers[name]; }
                ref_ptr<core::CPushHandler>& getPushConstant(const std::string& name) { return mPushConstants[name]; }

            protected:
                FMaterialParams m_fMatParams{};
                std::string m_srName;

                scope_ptr<core::descriptor::CDescriptorHandler> m_pDescriptorSet;
                std::map<std::string, ref_ptr<core::CHandler>> mBuffers;
                std::map<std::string, ref_ptr<core::CPushHandler>> mPushConstants;
                ref_ptr<core::pipeline::CPipelineBase> m_pPipeline;
                std::map<std::string, vk::DescriptorImageInfo> m_mTextures;
                std::map<std::string, vk::DescriptorBufferInfo> m_mBuffers;
            };
        }
    }

}