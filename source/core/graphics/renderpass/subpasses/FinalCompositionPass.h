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
                void create(std::shared_ptr<Scene::CRenderObject>& root) override;
                void render(vk::CommandBuffer& commandBuffer, std::shared_ptr<Scene::CRenderObject>& root) override;
                void cleanup() override;
            };
        }
    }
}