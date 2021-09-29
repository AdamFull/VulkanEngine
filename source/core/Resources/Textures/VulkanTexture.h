#pragma once
#include "Resources/ResourceBase.h"

namespace Engine
{
    class Device;

    class TextureBase : public ResourceBase
    {
    public:
        void Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform) override;
        void ReCreate(std::unique_ptr<Device>& device) override;
        void Bind(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer) override;
        void Destroy(std::unique_ptr<Device>& device) override;

        virtual void UpdateDescriptor();

        virtual vk::DescriptorImageInfo& GetDescriptor() { return descriptor; }
    protected:
        virtual void GenerateMipmaps(std::unique_ptr<Device>& device, vk::Image &image, uint32_t mipLevels, vk::Format format, vk::Extent3D sizes, vk::ImageAspectFlags aspectFlags);

        vk::Image               image;
        vk::ImageLayout         imageLayout;
        vk::DeviceMemory        deviceMemory;
        vk::ImageView           view;
        uint32_t                width, height, channels;
        uint32_t                mipLevels;
        uint32_t                layerCount;
        vk::DescriptorImageInfo descriptor;
        vk::Sampler             sampler;
    };

    class Texture2D : public TextureBase
    {
    public:
        void Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform) override;
        void ReCreate(std::unique_ptr<Device>& device) override;
        void Bind(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer) override;
        void Destroy(std::unique_ptr<Device>& device) override;

        void Load(std::unique_ptr<Device>& device, std::string srPath, uint32_t idx);
    };

    class TextureCubemap : public TextureBase
    {
    public:
        void Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform) override;
        void ReCreate(std::unique_ptr<Device>& device) override;
        void Bind(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer) override;
        void Destroy(std::unique_ptr<Device>& device) override;

        void Load();
    };
}