#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        namespace Texture { class TextureBase; }
        namespace Material { class MaterialBase; }
    }
    namespace Core
    {
        class Device;
        class SwapChain;

        struct FRenderSystem
        {
            std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> vCommandBuffers;
            uint32_t imageIndex{0};
            bool bFrameStarted{false};
        };

        class RenderSystem
        {
        public:
            RenderSystem() = default;
            RenderSystem(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapchain);
            ~RenderSystem();

            RenderSystem(const RenderSystem &) = delete;
            void operator=(const RenderSystem &) = delete;
            RenderSystem(RenderSystem &&) = delete;
            RenderSystem &operator=(RenderSystem &&) = delete;

            void Create();

            void ReCreate();
            void Cleanup();

            vk::CommandBuffer BeginFrame();
            vk::Result EndFrame();

            void BeginRender(vk::CommandBuffer& commandBuffer);
            void EndRender(vk::CommandBuffer& commandBuffer);

            void BeginPostProcess(vk::CommandBuffer& commandBuffer);
            void EndPostProcess(vk::CommandBuffer& commandBuffer);

            // Getters
            inline std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> &GetCommandBuffers() { return data.vCommandBuffers; }
            inline uint32_t GetImageIndex() { return data.imageIndex; }
            inline bool GetFrameStartFlag() { return data.bFrameStarted; }
            vk::CommandBuffer GetCurrentCommandBuffer() const;

        private:
            void Finalize(vk::CommandBuffer& commandBuffer);

            void RenderSystem::CreateCommandBuffers();
            FRenderSystem data;
            
            vk::Extent2D screenExtent;

            std::shared_ptr<Device> m_device;
            std::shared_ptr<SwapChain> m_swapchain;
        };
    }
}