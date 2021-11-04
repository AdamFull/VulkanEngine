#pragma once
#include "Resources/ResourceBase.h"
#include "Core/Pipeline/PipelineConfig.h"

namespace Engine
{
    namespace Resources
    {
        namespace Texture
        {
            class TextureBase;
        }
        namespace Generators
        {
            class GeneratorBase : public ResourceBase
            {
            public:
                virtual void Create();
                void ReCreate() override;
                void Update(uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;
                virtual std::shared_ptr<Texture::TextureBase> GetGenerated() { return m_pGenerated; }

            protected:
                virtual inline Core::Pipeline::EShaderSet GetShaderSet() { return Core::Pipeline::EShaderSet::eNone; }
                virtual void CreateRenderPass();
                virtual void CreateFramebuffer();
                virtual void CreateDescriptorSets(uint32_t images);

                std::shared_ptr<Texture::TextureBase> m_pGenerated;
                std::shared_ptr<Texture::TextureBase> m_pFramebufferImage;
                vk::RenderPass renderPass;
                vk::Framebuffer framebuffer;
            };
        }
    }
}