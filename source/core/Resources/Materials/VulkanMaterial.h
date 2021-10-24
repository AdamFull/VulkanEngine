#pragma once
#include "Resources/ResourceBase.h"
#include "Renderer/Pipeline/VulkanPipeline.h"
#include "Renderer/Pipeline/PipelineFactory.h"
#include "Resources/Textures/VulkanTexture.h"

namespace Engine
{
    class TextureBase;

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
        virtual void Create();
        virtual void AddTexture(ETextureAttachmentType eAttachment, std::shared_ptr<TextureBase> pTexture);
        virtual void AddTextures(std::map<ETextureAttachmentType, std::shared_ptr<TextureBase>> mTextures);
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;

        inline void SetParams(FMaterialParams&& params) { m_fMatParams = params; }
    protected:
        virtual inline EShaderSet GetShaderSet() { return EShaderSet::eNone; }
        FPipelineCreateInfo CreateInfo(EShaderSet eSet);
        void CreateDescriptorSetLayout();
        void CreateDescriptorPool(uint32_t images);
        void CreatePipelineLayout(uint32_t images);
        void CreatePipelineCache();
        virtual void CreateDescriptorSets(uint32_t images);

        FMaterialParams m_fMatParams{};
        std::vector<vk::DescriptorSet> matricesSet;
        std::vector<vk::DescriptorSet> skinsSet;
        std::vector<vk::DescriptorSet> texturesSet;

        vk::DescriptorSetLayout descriptorSetLayout;
        vk::DescriptorPool descriptorPool;
        vk::PipelineLayout pipelineLayout;
        vk::PipelineCache pipelineCache;
        std::shared_ptr<PipelineBase> pPipeline;
        std::map<ETextureAttachmentType, std::shared_ptr<TextureBase>> m_mTextures;
        static std::map<int, ETextureAttachmentType> m_mTextureBindings;
    };    
}