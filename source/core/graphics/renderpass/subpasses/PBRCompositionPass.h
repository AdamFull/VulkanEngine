#pragma once
#include "graphics/renderpass/Subpass.h"
#include "graphics/image/Image.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CPBRCompositionPass : public CSubpass
            {
            public:
                void create(std::unique_ptr<FRenderCreateInfo>& createInfo) override;
                void render(std::unique_ptr<FRenderProcessInfo>& renderData) override;
                void cleanup() override;

            protected:
                static std::shared_ptr<CImage> ComputeBRDFLUT(uint32_t size);
                static std::shared_ptr<CImage> ComputeIrradiance(const std::shared_ptr<CImage> &source, uint32_t size);
                static std::shared_ptr<CImage> ComputePrefiltered(const std::shared_ptr<CImage> &source, uint32_t size);
            private:
                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
                std::shared_ptr<CImage> m_pSkybox;

                utl::future<std::shared_ptr<CImage>> brdf;
                utl::future<std::shared_ptr<CImage>> irradiance;
                utl::future<std::shared_ptr<CImage>> prefiltered;
            };
        }
    }
}