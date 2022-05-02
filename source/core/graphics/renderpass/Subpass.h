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
            struct FRenderCreateInfo
            {
                FRenderCreateInfo() {}
                std::shared_ptr<Resources::CResourceManager> resourceManager;
                std::unordered_map<std::string, std::shared_ptr<CImage>> images;
                std::shared_ptr<Scene::Objects::RenderObject> root;
                vk::RenderPass renderPass;
                uint32_t subpass;
            };

            struct FRenderProcessInfo
            {
                FRenderProcessInfo() {}
                vk::CommandBuffer commandBuffer;
                std::unordered_map<std::string, std::shared_ptr<CImage>> images;
                std::shared_ptr<Scene::Objects::RenderObject> root;
            };

            class CSubpass
            {
            public:
                CSubpass() = default;

                virtual void create(std::unique_ptr<FRenderCreateInfo>& createInfo) {}
                virtual void render(std::unique_ptr<FRenderProcessInfo>& renderData) {}
                virtual void cleanup() {}
            protected:
                std::shared_ptr<CUniformBuffer> pUniform;
                std::shared_ptr<Resources::Material::CMaterialBase> pMaterial;
            };
        }
    }
}