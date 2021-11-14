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

        struct FRenderer
        {
            std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> vCommandBuffers;
            uint32_t imageIndex{0};
            bool bFrameStarted{false};
        };

        class Renderer
        {
        public:
            Renderer() = default;
            Renderer(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapchain);
            ~Renderer();

            Renderer(const Renderer &) = delete;
            void operator=(const Renderer &) = delete;
            Renderer(Renderer &&) = delete;
            Renderer &operator=(Renderer &&) = delete;

            void Create();

            void ReCreate();
            void Cleanup();

            vk::CommandBuffer BeginFrame();
            vk::Result EndFrame();

            void BeginRender(vk::CommandBuffer& commandBuffer);
            void EndRender(vk::CommandBuffer& commandBuffer);

            // Getters
            inline std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> &GetCommandBuffers() { return data.vCommandBuffers; }
            inline uint32_t GetImageIndex() { return data.imageIndex; }
            inline bool GetFrameStartFlag() { return data.bFrameStarted; }
            vk::CommandBuffer GetCurrentCommandBuffer() const;

        private:
            void Finalize(vk::CommandBuffer& commandBuffer);

            void Renderer::CreateCommandBuffers();
            FRenderer data;
            
            vk::Extent2D screenExtent;

            std::shared_ptr<Device> m_device;
            std::shared_ptr<SwapChain> m_swapchain;
        };
    }
}