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
                void create(std::shared_ptr<Scene::CRenderObject>& root) override;
                void render(vk::CommandBuffer& commandBuffer, std::shared_ptr<Scene::CRenderObject>& root) override;
                void cleanup() override;
            private:
                int direction{-1};
            };
        }
    }
}