#pragma once
#include "DataTypes/VulkanSwapChainSipportDetails.h"
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

        struct FSwapChain
        {
            vk::Format imageFormat;
            vk::Format depthFormat;
            vk::Extent2D extent;

            std::vector<vk::Framebuffer> vFramebuffers;
            vk::RenderPass renderPass;

            //For deferred rendering
            std::vector<vk::Framebuffer> vOffscreenFramebuffers;
            vk::RenderPass offscreenRenderPass;

            // Swap chain buffer
            std::vector<vk::Image> vImages;
            std::vector<vk::ImageView> vImageViews;

            // Depth buffer
            vk::Image depthImage;
            vk::DeviceMemory depthImageMemory;
            vk::ImageView depthImageView;

            // LveDevice &device;
            vk::Extent2D windowExtent;

            vk::SwapchainKHR swapChain;
            // std::shared_ptr<LveSwapChain> oldSwapChain;

            std::vector<vk::Semaphore> vImageAvailableSemaphores;
            std::vector<vk::Semaphore> vRenderFinishedSemaphores;
            std::vector<vk::Fence> vInFlightFences;
            size_t currentFrame = 0;
            uint32_t iFramesInFlight = 2;
        };

        struct FAttachmentInfo
        {
            FAttachmentInfo(vk::Format f, vk::ImageUsageFlags u, vk::ClearColorValue c, vk::ClearDepthStencilValue d) :
            format(f), usage(u), color(c), depth(d) {}
            vk::Format format{vk::Format::eUndefined};
            vk::ImageUsageFlags usage{};
            vk::ClearColorValue color{};
            vk::ClearDepthStencilValue depth{};
        };

        class SwapChain
        {
        public:
            SwapChain(std::shared_ptr<Device> device);
            ~SwapChain();

            SwapChain(const SwapChain &) = delete;
            void operator=(const SwapChain &) = delete;
            SwapChain(SwapChain &&) = delete;
            SwapChain &operator=(SwapChain &&) = delete;

            void Create();

            vk::Result AcquireNextImage(uint32_t *imageIndex);
            vk::Result SubmitCommandBuffers(const vk::CommandBuffer *commandBuffer, uint32_t *imageIndex);

            void Cleanup();
            void ReCreate();

            inline void SetFramesInFlight(uint32_t iValue) { data.iFramesInFlight = iValue; }

            inline float GetAspectRatio()
            {
                return static_cast<float>(data.extent.width) / static_cast<float>(data.extent.height);
            }

            // Getters
            inline vk::Format GetImageFormat() { return data.imageFormat; };
            inline vk::Format GetDepthFormat() { return data.depthFormat; }
            inline vk::Extent2D GetExtent() { return data.extent; }

            inline std::vector<vk::Framebuffer> &GetFramebuffers() { return data.vFramebuffers; }
            inline vk::RenderPass &GetRenderPass() { return data.renderPass; }

            inline std::vector<vk::Framebuffer> &GetOffscreenFramebuffers() { return data.vOffscreenFramebuffers; }
            inline vk::RenderPass &GetOffscreenRenderPass() { return data.offscreenRenderPass; }

            std::shared_ptr<Resources::Material::MaterialBase> GetComposition() { return m_pComposition; }

            inline std::vector<vk::Image> &GetImages() { return data.vImages; }
            inline std::vector<vk::ImageView> &GetImageViews() { return data.vImageViews; }

            inline vk::Image &GetDepthImage() { return data.depthImage; }
            inline vk::DeviceMemory &GetDepthImageMemory() { return data.depthImageMemory; }
            inline vk::ImageView &GetDepthImageView() { return data.depthImageView; }

            inline vk::Extent2D GetWindowExtent() { return data.windowExtent; }

            inline vk::SwapchainKHR &GetSwapChain() { return data.swapChain; }

            inline std::vector<vk::Semaphore> &GetImageAvailableSemaphores() { return data.vImageAvailableSemaphores; }
            inline std::vector<vk::Semaphore> &GetRenderFinishedSemaphores() { return data.vRenderFinishedSemaphores; }
            inline std::vector<vk::Fence> &GetInFlightFences() { return data.vInFlightFences; }
            inline size_t GetCurrentFrame() { return data.currentFrame; }
            inline uint32_t GetFramesInFlight() { return data.iFramesInFlight; }

            static std::map<Resources::ETextureAttachmentType, FAttachmentInfo> vAttachments;

        private:
            void CreateSwapChain();
            void CreateSwapChainImageViews();
            void CreateDepthResources();
            void CreateRenderPass();
            void CreateOffscreenRenderPass();
            void CreateFrameBuffers();
            void CreateOffscreenFrameBuffers();
            std::shared_ptr<Resources::Texture::TextureBase> CreateOffscreenImage(vk::Format format, vk::ImageUsageFlags usage);
            void CreateOffscreenImages();
            void CreateCompositionMaterial();
            void ReCreateCompositionMaterial();
            void CreateSyncObjects();

            std::map<Resources::ETextureAttachmentType, std::shared_ptr<Resources::Texture::TextureBase>> m_mGBuffer;
            std::shared_ptr<Resources::Material::MaterialBase> m_pComposition;

            std::shared_ptr<Device> m_device;

            vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &);

            FSwapChain data;
        };
    }
}