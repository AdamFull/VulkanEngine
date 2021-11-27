#pragma once
#include "RendererBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Rendering
        {
            class EnvironmentRenderer : public RendererBase
            {
            public:
                EnvironmentRenderer() = default;

                void Create(std::shared_ptr<Resources::ResourceManager> pResMgr) override;
                void ReCreate(uint32_t framesInFlight) override;
                void Render(vk::CommandBuffer& commandBuffer) override;
                void Cleanup() override;

            private:
                std::shared_ptr<UniformBuffer> m_pUniform;
            };
        }
    }
}