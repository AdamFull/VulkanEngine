#pragma once
#include "RendererBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Rendering
        {
            class DeferredRenderer : public RendererBase
            {
            public:
                DeferredRenderer() = default;

                void Create(std::shared_ptr<Resources::ResourceManager> pResMgr) override;
                void Render(vk::CommandBuffer& commandBuffer) override;
                void Cleanup() override;
            };
        }
    }
}