#pragma once
#include "util/threading.hpp"
#include "graphics/renderpass/Subpass.h"
#include "graphics/image/Image.h"
#include "graphics/scene/objects/components/light/LightSourceBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CPBRCompositionPass : public CSubpass
            {
            public:
                void create() override;
                void reCreate() override;
                void render(vk::CommandBuffer& commandBuffer) override;
                void cleanup() override;

            protected:
                static std::shared_ptr<CImage> ComputeBRDFLUT(uint32_t size);
                static std::shared_ptr<CImage> ComputeIrradiance(const std::shared_ptr<CImage> &source, uint32_t size);
                static std::shared_ptr<CImage> ComputePrefiltered(const std::shared_ptr<CImage> &source, uint32_t size);
            private:
                std::shared_ptr<CImage> m_pSkybox;

                CFuture<std::shared_ptr<CImage>> brdf;
                CFuture<std::shared_ptr<CImage>> irradiance;
                CFuture<std::shared_ptr<CImage>> prefiltered;

                std::array<Scene::FPointLight, 32> point_lights;
                int32_t point_count{0};

                std::array<Scene::FDirectionalLight, 32> directional_lights;
                int32_t directional_count{0};

                std::array<Scene::FSpotLight, 32> spot_lights;
                int32_t spot_count{0};
            };
        }
    }
}