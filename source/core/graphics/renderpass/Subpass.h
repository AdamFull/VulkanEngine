#pragma once
#include "resources/ResourceManager.h"
#include "graphics/image/Image.h"
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material { class MaterialBase; }
    }
    namespace Core
    {
        class UniformBuffer;
        namespace Render
        {
            struct FRenderCreateInfo
            {
                FRenderCreateInfo() {}
                std::shared_ptr<Resources::ResourceManager> resourceManager;
                std::unordered_map<std::string, std::shared_ptr<Image>> images;
                std::shared_ptr<Scene::Objects::RenderObject> root;
                vk::RenderPass renderPass;
                uint32_t subpass;
            };

            struct FRenderProcessInfo
            {
                FRenderProcessInfo() {}
                vk::CommandBuffer commandBuffer;
                std::unordered_map<std::string, std::shared_ptr<Image>> images;
                std::shared_ptr<Scene::Objects::RenderObject> root;
            };

            class CSubpass
            {
            public:
                CSubpass() = default;

                virtual void create(std::shared_ptr<FRenderCreateInfo> createData) {}
                virtual void render(std::shared_ptr<FRenderProcessInfo> renderData) {}
                virtual void cleanup() {}
            private:
            };
        }
    }
}