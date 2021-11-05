#pragma once
#include "Core/Pipeline/PipelineConfig.h"

namespace Engine
{
    namespace Core
    {
        namespace Pipeline { class PipelineBase; }
        namespace Descriptor { class VulkanDescriptorSetContainer; class VulkanDescriptorPool; }
    }
    namespace Resources
    {
        namespace Texture
        {
            class TextureBase;
        }
        namespace Material
        {
            namespace Generator
            {
                enum class EGeneratorType
                {
                    eBRDF,
                    eIrradiate,
                    ePrefiltred
                };

                struct FGeneratorTemporary
                {
                    vk::RenderPass renderPass;
                    vk::Framebuffer framebuffer;
                    std::shared_ptr<Core::Descriptor::VulkanDescriptorSetContainer> m_pMatDesc;
                    std::shared_ptr<Core::Descriptor::VulkanDescriptorPool> m_pDescriptorPool;
                    vk::PipelineLayout pipelineLayout;
                    vk::PipelineCache pipelineCache;
                };

                class GeneratorBase
                {
                public:
                    static std::shared_ptr<Texture::TextureBase> Generate(EGeneratorType eType, uint32_t dimension);
                private:
                    static vk::RenderPass CreateRenderPass(vk::Format format);
                    static vk::Framebuffer CreateFramebuffer(std::shared_ptr<Texture::TextureBase> pTexture, std::shared_ptr<FGeneratorTemporary> pTemp);
                    static vk::PipelineLayout CreatePipelineLayout(std::shared_ptr<FGeneratorTemporary> pTemp);
                    static vk::PipelineCache CreatePipelineCache(std::shared_ptr<FGeneratorTemporary> pTemp);
                    static std::shared_ptr <Core::Pipeline::PipelineBase> CreatePipeline(std::shared_ptr<FGeneratorTemporary> pTemp, Core::Pipeline::EShaderSet eSet);
                };
            }
        }
    }
}