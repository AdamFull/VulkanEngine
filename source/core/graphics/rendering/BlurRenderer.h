#pragma once
#include "RendererBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Rendering
        {
            class BlurRenderer : public RendererBase
            {
            public:
                BlurRenderer() = default;

                void Create(std::shared_ptr<Resources::ResourceManager> pResMgr) override;
                void ReCreate(uint32_t framesInFlight) override;
                void Render(vk::CommandBuffer& commandBuffer) override;
                void Cleanup() override;

                void AddProduct(texture_type_t eAttach, std::shared_ptr<Image> image) override;
                void SetUniform(void* uniform) override;
            protected:
                void CreateMaterial(std::shared_ptr<Resources::ResourceManager> pResMgr);

            protected:
                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
            };
        }
    }
}