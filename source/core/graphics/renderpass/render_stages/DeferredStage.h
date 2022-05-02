#pragma once
#include "graphics/renderpass/RenderStage.h"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CDeferredStage : public CRenderStage
            {
            public:
                ~CDeferredStage();
                void create(std::shared_ptr<Resources::CResourceManager>& resourceManager, std::shared_ptr<Scene::CRenderObject>& root) override;
                void reCreate() override;
                void render(vk::CommandBuffer& commandBuffer, std::shared_ptr<Scene::CRenderObject>& root) override;
                void cleanup() override;
            private:
            };
        }
    }
}