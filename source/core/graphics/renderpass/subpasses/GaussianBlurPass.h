#pragma once
#include "graphics/renderpass/Subpass.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CGaussianBlurPass : public CSubpass
            {
            public:
                CGaussianBlurPass() = default;
                explicit CGaussianBlurPass(int direction) : direction(direction) {}
                void create(std::shared_ptr<FRenderCreateInfo> createData) override;
                void render(std::shared_ptr<FRenderProcessInfo> renderData) override;
                void cleanup() override;
            private:
                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
                int direction{-1};
            };
        }
    }
}