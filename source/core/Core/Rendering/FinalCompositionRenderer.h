#pragma once
#include "RendererBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Rendering
        {
            class BlurRenderer;
            class FinalCompositionRenderer : public RendererBase
            {
            public:
                FinalCompositionRenderer() = default;

                void Create(std::shared_ptr<Resources::ResourceManager> pResMgr) override;
                void ReCreate(uint32_t framesInFlight) override;
                void Render(vk::CommandBuffer& commandBuffer) override;
                void Cleanup() override;
            protected:
                void CreateRenderPass() override;
                void CreateFramebuffers() override;
                void CreateMaterial(std::shared_ptr<Resources::ResourceManager> pResMgr);

            private:
                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
                std::shared_ptr<BlurRenderer> m_pVertBlur;
                std::shared_ptr<BlurRenderer> m_pHorizBlur;
            };
        }
    }
}