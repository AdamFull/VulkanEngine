#pragma once
#include "Core/Pipeline/VulkanPipeline.h"
#include "Core/Pipeline/PipelineFactory.h"
#include "Resources/Textures/VulkanTexture.h"

#include "Core/Descriptor/DescriptorPool.h"
#include "Core/Descriptor/DescriptorSetContainer.h"

#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
        namespace Texture { class TextureBase; }
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
                MaterialBase(std::shared_ptr<Core::Device> device, std::shared_ptr<Core::SwapChain> swapchain);
                virtual ~MaterialBase();

                virtual void Create(std::shared_ptr<ResourceManager> pResMgr);
                virtual void AddTexture(ETextureAttachmentType eAttachment, std::shared_ptr<Texture::TextureBase> pTexture);
                std::shared_ptr<Texture::TextureBase> GetTexture(ETextureAttachmentType eAttachment);
                virtual void AddTextures(std::map<ETextureAttachmentType, std::shared_ptr<Texture::TextureBase>> mTextures);
                virtual void ReCreate();
                virtual void Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex);
                virtual void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
                virtual void Cleanup();

                inline void SetParams(FMaterialParams &&params) { m_fMatParams = params; }

                inline void SetName(const std::string& srName) { m_srName = srName; }
                inline std::string GetName() { return m_srName; }

            protected:
                virtual inline Core::Pipeline::EShaderSet GetShaderSet() { return Core::Pipeline::EShaderSet::eNone; }
                virtual Core::Pipeline::FPipelineCreateInfo CreateInfo(Core::Pipeline::EShaderSet eSet);
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
                std::shared_ptr<Core::Pipeline::PipelineBase> pPipeline;
                std::map<ETextureAttachmentType, std::shared_ptr<Texture::TextureBase>> m_mTextures;

                std::shared_ptr<Core::Device> m_device;
                std::shared_ptr<Core::SwapChain> m_swapchain;
            };
        }
    }

}