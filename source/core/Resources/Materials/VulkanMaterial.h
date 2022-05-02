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
        class CResourceManager;
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

            class CMaterialBase 
            {
            public:
                CMaterialBase() = default;
                virtual ~CMaterialBase();

                virtual void create(vk::RenderPass& renderPass, uint32_t subpass);
                void addTexture(const std::string& attachment, vk::DescriptorImageInfo& descriptor);
                void addTexture(const std::string& attachment, std::shared_ptr<Core::CImage> pTexture);
                vk::DescriptorImageInfo& getTexture(const std::string& attachment);
                virtual void reCreate();
                virtual void update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex);
                virtual void bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
                virtual void cleanup();

                inline void setParams(FMaterialParams &&params) { m_fMatParams = params; }

                void setName(const std::string& srName);
                inline std::string getName() { return m_srName; }
                inline std::shared_ptr<Core::Pipeline::CPipelineBase> getPipeline() { return m_pPipeline; }

            protected:
                FMaterialParams m_fMatParams{};
                std::string m_srName;

                std::unique_ptr<Core::Descriptor::CDescriptorHandler> m_pDescriptorSet;
                std::vector<std::shared_ptr<Core::CUniformHandler>> m_vUniformBuffers;
                std::vector<std::shared_ptr<Core::CPushHandler>> m_vPushConstants;
                std::shared_ptr<Core::Pipeline::CPipelineBase> m_pPipeline;
                std::map<std::string, vk::DescriptorImageInfo> m_mTextures;
            };
        }
    }

}