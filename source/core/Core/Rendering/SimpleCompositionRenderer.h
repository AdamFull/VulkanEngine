#pragma once
#include "RendererBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Rendering
        {
            class SimpleCompositionRenderer : public RendererBase
            {
            public:
                SimpleCompositionRenderer() = default;

                void Create(std::shared_ptr<Resources::ResourceManager> pResMgr) override;
                void ReCreate(uint32_t framesInFlight) override;
                void Render(vk::CommandBuffer& commandBuffer) override;
                void Cleanup() override;
            protected:
                void CreateMaterial(std::shared_ptr<Resources::ResourceManager> pResMgr);

            private:
                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
            };
        }
    }
}