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
                void create(std::shared_ptr<Resources::ResourceManager> resourceManager, std::shared_ptr<Scene::Objects::RenderObject> root, vk::RenderPass& renderPass, uint32_t subpass) override;
                void render(vk::CommandBuffer& commandBuffer, std::shared_ptr<Scene::Objects::RenderObject> root) override;
                void cleanup() override;
            private:
                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
                int direction{-1};
            };
        }
    }
}