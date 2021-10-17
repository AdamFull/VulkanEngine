#pragma once
#include "Resources/ResourceBase.h"
#include "Renderer/Pipeline/VulkanPipeline.h"
#include "Renderer/Pipeline/PipelineFactory.h"
#include "Resources/Textures/VulkanTexture.h"

namespace Engine
{
    class TextureBase;

    class MaterialBase : public ResourceBase
    {
    public:
        virtual void Create();
        virtual void AddTexture(ETextureAttachmentType eAttachment, std::shared_ptr<TextureBase> pTexture);
        virtual void AddTextures(std::map<ETextureAttachmentType, std::shared_ptr<TextureBase>> mTextures);
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;

    protected:
        virtual inline EShaderSet GetShaderSet() { return EShaderSet::eNone; }
        virtual void CreateDescriptorSets(uint32_t images);

        std::vector<vk::DescriptorSet> vDescriptorSets;
        std::shared_ptr<PipelineBase> pPipeline;
        std::map<ETextureAttachmentType, std::shared_ptr<TextureBase>> m_mTextures;
    };    
}