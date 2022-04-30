#pragma once
#include "graphics/renderpass/Subpass.h"
#include "graphics/image/Image.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CPBRCompositionPass : public CSubpass
            {
            public:
                void create(std::shared_ptr<Resources::ResourceManager> resourceManager, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<Scene::Objects::RenderObject> root, vk::RenderPass& renderPass, uint32_t subpass) override;
                void render(vk::CommandBuffer& commandBuffer, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<Scene::Objects::RenderObject> root) override;
                void cleanup() override;

            protected:
                static std::shared_ptr<Image> ComputeBRDFLUT(uint32_t size);
                static std::shared_ptr<Image> ComputeIrradiance(const std::shared_ptr<Image> &source, uint32_t size);
                static std::shared_ptr<Image> ComputePrefiltered(const std::shared_ptr<Image> &source, uint32_t size);
            private:
                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
                std::shared_ptr<Image> m_pSkybox;

                utl::future<std::shared_ptr<Image>> brdf;
                utl::future<std::shared_ptr<Image>> irradiance;
                utl::future<std::shared_ptr<Image>> prefiltered;
            };
        }
    }
}