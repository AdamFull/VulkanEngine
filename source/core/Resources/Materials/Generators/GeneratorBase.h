#pragma once
#include "Resources/Materials/VulkanMaterial.h"

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
            namespace Generator
            {
                class GeneratorBase : public MaterialBase
                {
                public:
                    ~GeneratorBase() override;

                    void Create() override;
                    void Cleanup() override;

                    virtual void Generate(uint32_t indexCount, uint32_t firstIndex);
                    virtual std::shared_ptr<Texture::TextureBase> Get();
                protected:
                    virtual inline Core::Pipeline::EShaderSet GetShaderSet() override { return Core::Pipeline::EShaderSet::eNone; }
                    Core::Pipeline::FPipelineCreateInfo CreateInfo(Core::Pipeline::EShaderSet eSet) override;
                    virtual void CreateDescriptors(uint32_t images) override;
                    void CreateRenderPass(vk::Format format);
                    void CreateFramebuffer();
                    virtual void CreateTextures();

                protected:
                    std::shared_ptr<Texture::TextureBase> m_pGeneratedImage;
                    vk::Framebuffer framebuffer;
                    vk::Format imageFormat;
                    uint32_t m_iDimension{512};
                    uint32_t m_iInternalFormat{0x822F};
                };
            }
        }
    }
}