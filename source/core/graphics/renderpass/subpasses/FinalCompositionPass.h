#pragma once
#include "graphics/renderpass/Subpass.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CFinalCompositionPass : public CSubpass
            {
            public:
                void create(std::unique_ptr<FRenderCreateInfo>& createInfo) override;
                void render(std::unique_ptr<FRenderProcessInfo>& renderData) override;
                void cleanup() override;
            private:
                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
            };
        }
    }
}