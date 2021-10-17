#pragma once
#include "Resources/ResourceBase.h"
#include "Renderer/Pipeline/PipelineManager.h"

namespace Engine
{
    class TextureBase;

    class GeneratorBase : public ResourceBase
    {
    public:
        virtual void Create();
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;
        virtual std::shared_ptr<TextureBase> GetGenerated() { return m_pGenerated; }

    protected:
        virtual inline EShaderSet GetShaderSet() { return EShaderSet::eNone; }
        virtual void CreateRenderPass();
        virtual void CreateFramebuffer();
        virtual void CreateDescriptorSets(uint32_t images);

        std::shared_ptr<TextureBase> m_pGenerated;
        std::shared_ptr<TextureBase> m_pFramebufferImage;
        vk::RenderPass renderPass;
        vk::Framebuffer framebuffer;
    };
}