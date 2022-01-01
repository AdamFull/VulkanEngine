#pragma once
#include "Core/Pipeline/VulkanPipeline.h"
#include "Core/Pipeline/PipelineFactory.h"
#include "Core/Image/Image.h"

#include "Core/Descriptor/DescriptorHandler.h"

#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Core
    {
        class Image;
    }
    namespace Resources
    {
        class ResourceManager;
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

            class MaterialBase 
            {
            public:
                MaterialBase() = default;
                virtual ~MaterialBase();

                virtual void Create(std::shared_ptr<ResourceManager> pResMgr, vk::RenderPass& rPass);
                virtual void Create(std::shared_ptr<ResourceManager> pResMgr);
                void AddTexture(ETextureAttachmentType eAttachment, vk::DescriptorImageInfo& descriptor);
                void AddTexture(ETextureAttachmentType eAttachment, std::shared_ptr<Core::Image> pTexture);
                vk::DescriptorImageInfo& GetTexture(ETextureAttachmentType eAttachment);
                virtual void ReCreate();
                virtual void Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex);
                virtual void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
                virtual void Cleanup();

                inline void SetParams(FMaterialParams &&params) { m_fMatParams = params; }

                void SetName(const std::string& srName);
                inline std::string GetName() { return m_srName; }

            protected:
                FMaterialParams m_fMatParams{};
                std::string m_srName;

                std::unique_ptr<Core::Descriptor::DescriptorHandler> m_pDescriptorSet;

                vk::RenderPass renderPass;
                Core::Pipeline::FPipelineInitial initial;
                std::shared_ptr<Core::Pipeline::PipelineBase> m_pPipeline;
                std::map<ETextureAttachmentType, vk::DescriptorImageInfo> m_mTextures;
            };
        }
    }

}