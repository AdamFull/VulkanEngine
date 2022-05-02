#pragma once
#include "resources/ResourceManager.h"
#include "graphics/image/Image.h"
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material { class CMaterialBase; }
    }
    namespace Core
    {
        class CUniformBuffer;
        namespace Render
        {
            class CSubpass
            {
            public:
                CSubpass() = default;

                virtual void create(std::shared_ptr<Resources::CResourceManager>& resourceManager, std::shared_ptr<Scene::CRenderObject>& root, vk::RenderPass& renderPass, uint32_t subpass) {}
                virtual void render(vk::CommandBuffer& commandBuffer, std::unordered_map<std::string, std::shared_ptr<CImage>>& images, std::shared_ptr<Scene::CRenderObject>& root) {}
                virtual void cleanup() {}
            protected:
                std::shared_ptr<CUniformBuffer> pUniform;
                std::shared_ptr<Resources::Material::CMaterialBase> pMaterial;
            };
        }
    }
}