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
                void create(std::shared_ptr<Resources::ResourceManager> resourceManager, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<Scene::Objects::RenderObject> root, vk::RenderPass& renderPass, uint32_t subpass) override;
                void render(vk::CommandBuffer& commandBuffer, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<Scene::Objects::RenderObject> root) override;
                void cleanup() override;
            private:
                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
            };
        }
    }
}