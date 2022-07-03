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
                float alphaCutoff{0.5f};
                glm::vec3 emissiveFactor{0.f};
                float normalScale{1.f};
                float occlusionStrenth{1.f};
                glm::vec4 baseColorFactor{1.0f};
                float metallicFactor{1.f};
                float roughnessFactor{1.f};
                float tessellationFactor{0.f};
	            float tessStrength{0.f};
            };

            struct FMaterialUniqueObjects
            {
                utl::scope_ptr<core::descriptor::CDescriptorHandler> pDescriptorSet;
                std::map<std::string, utl::ref_ptr<core::CHandler>> mBuffers;
                std::map<std::string, utl::ref_ptr<core::CPushHandler>> mPushConstants;
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
                void addTexture(const std::string& attachment, utl::ref_ptr<core::CImage>& pTexture);

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
                virtual void bind(vk::CommandBuffer& commandBuffer);

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

                void setInstances(uint32_t instance);

                void incrementUsageCount();

                void addDefinition(const std::string& definition, const std::string& value);

                /**
                 * @brief Get material name
                 * 
                 * @return std::string Material name
                 */
                inline std::string getName() { return m_srName; }

                /**
                 * @brief Get material pipeline object
                 * 
                 * @return utl::ref_ptr<Core::Pipeline::CPipelineBase> Pipeline smart pointer object
                 */
                inline utl::ref_ptr<core::pipeline::CPipelineBase>& getPipeline() { return pPipeline; }

                FMaterialParams& getParams() { return m_fMatParams; }

                utl::ref_ptr<core::CHandler>& getUniformBuffer(const std::string& name);
                utl::ref_ptr<core::CPushHandler>& getPushConstant(const std::string& name);
                std::map<std::string, utl::ref_ptr<core::CHandler>>& getUniformBuffers() { return vInstances.at(currentInstance)->mBuffers; }
                utl::scope_ptr<core::descriptor::CDescriptorHandler>& getDescriptorSet() { return vInstances.at(currentInstance)->pDescriptorSet; }

                size_t getTextureCount() { return mTextures.size(); }

            protected:
                FMaterialParams m_fMatParams{};
                std::string m_srName;
                uint32_t instances{1};
                uint32_t currentInstance{0};
                bool bIsCreated{false}, bIsReCreated{false};

                utl::ref_ptr<core::CHandler> pEmptyHamdler{nullptr};
                utl::ref_ptr<core::CPushHandler> pEmptyPushConstant{nullptr};
                std::vector<utl::scope_ptr<FMaterialUniqueObjects>> vInstances;
                utl::ref_ptr<core::pipeline::CPipelineBase> pPipeline;
                std::map<std::string, vk::DescriptorImageInfo> mTextures;
            };
        }
    }

}