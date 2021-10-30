#pragma once
#include "Resources/ResourceBase.h"
#include "Renderer/Pipeline/VulkanPipeline.h"
#include "Renderer/Pipeline/PipelineFactory.h"
#include "Resources/Textures/VulkanTexture.h"

#include "Renderer/Descriptor/DescriptorPool.h"
#include "Renderer/Descriptor/DescriptorSetContainer.h"

namespace Engine
{
    namespace Resources
    {
        namespace Texture
        {
            class TextureBase;
        }
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

            class MaterialBase : public ResourceBase
            {
            public:
                void Create() override;
                virtual void AddTexture(ETextureAttachmentType eAttachment, std::shared_ptr<Texture::TextureBase> pTexture);
                virtual void AddTextures(std::map<ETextureAttachmentType, std::shared_ptr<Texture::TextureBase>> mTextures);
                void ReCreate() override;
                void Update(uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;

                inline void SetParams(FMaterialParams &&params) { m_fMatParams = params; }

            protected:
                virtual inline Core::Pipeline::EShaderSet GetShaderSet() { return Core::Pipeline::EShaderSet::eNone; }
                Core::Pipeline::FPipelineCreateInfo CreateInfo(Core::Pipeline::EShaderSet eSet);
                void CreateDescriptorPool(uint32_t images);
                void CreatePipelineLayout(uint32_t images);
                void CreatePipelineCache();
                virtual void CreateDescriptors(uint32_t images);

                FMaterialParams m_fMatParams{};

                std::unique_ptr<Core::Descriptor::VulkanDescriptorSetContainer> m_pMatDesc;
                std::shared_ptr<Core::Descriptor::VulkanDescriptorPool> m_pDescriptorPool;

                vk::PipelineLayout pipelineLayout;
                vk::PipelineCache pipelineCache;
                std::shared_ptr<Core::Pipeline::PipelineBase> pPipeline;
                std::map<ETextureAttachmentType, std::shared_ptr<Texture::TextureBase>> m_mTextures;
                static std::map<int, ETextureAttachmentType> m_mTextureBindings;
            };
        }
    }

}