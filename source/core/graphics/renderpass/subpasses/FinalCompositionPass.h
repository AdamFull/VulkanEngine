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
                void create(std::shared_ptr<Resources::CResourceManager>& resourceManager, std::shared_ptr<Scene::CRenderObject>& root, vk::RenderPass& renderPass, uint32_t subpass) override;
                void render(vk::CommandBuffer& commandBuffer, std::unordered_map<std::string, std::shared_ptr<CImage>>& images, std::shared_ptr<Scene::CRenderObject>& root) override;
                void cleanup() override;
            };
        }
    }
}