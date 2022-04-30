#pragma once

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
        namespace Material { class MaterialBase; }
    }
    namespace Core
    {
        class UniformBuffer;
        class Image;
        namespace Scene {namespace Objects { class RenderObject; }}
        namespace Render
        {
            struct FRenderCreateInfo
            {
                std::shared_ptr<Resources::ResourceManager> resourceManager;
                std::vector<std::shared_ptr<Image>>& images;
                std::shared_ptr<Scene::Objects::RenderObject> root;
                vk::RenderPass& renderPass;
                uint32_t subpass;
            };

            class CSubpass
            {
            public:
                CSubpass() = default;

                virtual void create(std::shared_ptr<Resources::ResourceManager> resourceManager, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<Scene::Objects::RenderObject> root, vk::RenderPass& renderPass, uint32_t subpass) {}
                virtual void render(vk::CommandBuffer& commandBuffer, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<Scene::Objects::RenderObject> root) {}
                virtual void cleanup() {}
            private:
            };
        }
    }
}