#pragma once
#include "RendererBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Renderer
        {
            class RendererPostProcess : public RendererBase
            {
            public:
                RendererPostProcess(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapchain);

                void Create() override;
                void ReCreate() override;
                void Update(vk::CommandBuffer& commandBuffer, void* ubo) override;
                void Cleanup() override;
            
            protected:
                void CreateRenderPass() override;
                void CreateFrameBuffers() override;
                void CreateUniformBuffers() override;
                void CreateMaterial() override;
            };
        }
    }
}