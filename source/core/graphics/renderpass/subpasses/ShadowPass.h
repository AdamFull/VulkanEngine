#pragma once
#include "graphics/renderpass/Subpass.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CShadowPass : public CSubpass
            {
            public:
                void create(std::shared_ptr<FRenderCreateInfo> createData) override;
                void render(std::shared_ptr<FRenderProcessInfo> renderData) override;
                void cleanup() override;
            private:
                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
            };
        }
    }
}