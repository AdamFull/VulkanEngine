#pragma once
#include "Core/Pipeline/VulkanPipeline.h"
#include "Core/Pipeline/PipelineFactory.h"
#include "Resources/Textures/Image.h"

#include "Core/Descriptor/DescriptorPool.h"
#include "Core/Descriptor/DescriptorSetContainer.h"
#include "Core/Descriptor/DescriptorWriter.h"

#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
        namespace Texture { class Image; }
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

                virtual void Create(std::shared_ptr<ResourceManager> pResMgr);
                void AddTexture(ETextureAttachmentType eAttachment, vk::DescriptorImageInfo& descriptor);
                void AddTexture(ETextureAttachmentType eAttachment, std::shared_ptr<Texture::Image> pTexture);
                vk::DescriptorImageInfo& GetTexture(ETextureAttachmentType eAttachment);
                virtual void ReCreate();
                virtual void Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex);
                virtual void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
                virtual void Cleanup();

                inline void SetParams(FMaterialParams &&params) { m_fMatParams = params; }

                void SetName(const std::string& srName);
                inline std::string GetName() { return m_srName; }

            protected:
                void CreateDescriptorPool(uint32_t images);
                void CreatePipelineLayout(uint32_t images);
                void CreatePipelineCache();
                virtual void CreateDescriptors(uint32_t images);

                FMaterialParams m_fMatParams{};
                std::string m_srName;

                std::unique_ptr<Core::Descriptor::VulkanDescriptorSetContainer> m_pMatDesc;
                std::shared_ptr<Core::Descriptor::VulkanDescriptorPool> m_pDescriptorPool;
                std::vector<vk::PushConstantRange> m_vConstantRanges;

                vk::RenderPass renderPass;
                vk::PipelineLayout pipelineLayout;
                vk::PipelineCache pipelineCache;
                Core::Pipeline::FPipelineInitial initial;
                std::shared_ptr<Core::Pipeline::PipelineBase> pPipeline;
                std::map<ETextureAttachmentType, vk::DescriptorImageInfo> m_mTextures;
            };
        }
    }

}