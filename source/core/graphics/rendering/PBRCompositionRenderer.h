#pragma once
#include "RendererBase.h"
#include "graphics/image/Image.h"

namespace Engine
{
    namespace Core
    {
        namespace Rendering
        {
            class PBRCompositionRenderer : public RendererBase
            {
            public:
                PBRCompositionRenderer() = default;

                void Create(std::shared_ptr<Resources::ResourceManager> pResMgr) override;
                void ReCreate(uint32_t framesInFlight) override;
                void Render(vk::CommandBuffer& commandBuffer) override;
                void Cleanup() override;
            protected:
                void CreateMaterial(std::shared_ptr<Resources::ResourceManager> pResMgr);
                static std::shared_ptr<Image> ComputeBRDFLUT(uint32_t size);
                static std::shared_ptr<Image> ComputeIrradiance(const std::shared_ptr<Image> &source, uint32_t size);
                static std::shared_ptr<Image> ComputePrefiltered(const std::shared_ptr<Image> &source, uint32_t size);

            private:
                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
                std::shared_ptr<Image> m_pSkybox;

                utl::future<std::shared_ptr<Image>> brdf;
                utl::future<std::shared_ptr<Image>> irradiance;
                utl::future<std::shared_ptr<Image>> prefiltered;
            };
        }
    }
}