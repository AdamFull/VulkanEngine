#pragma once
#include "Resources/ResourceBase.h"

namespace Engine
{
    class Device;

    class TextureBase : public ResourceBase
    {
    public:
        void Create(std::string srResourcePath) override;
        void ReCreate() override;
        void Update(uint32_t imageIndex) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Destroy() override;

        virtual void UpdateDescriptor();

        virtual vk::DescriptorImageInfo& GetDescriptor() { return descriptor; }
    protected:
        virtual void GenerateMipmaps(vk::Image &image, uint32_t mipLevels, vk::Format format, vk::Extent3D sizes, vk::ImageAspectFlags aspectFlags);

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
        void Create(std::string srResourcePath) override;
        void ReCreate() override;
        void Update(uint32_t imageIndex) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Destroy() override;

        void Load(std::string srPath);
    };

    class TextureCubemap : public TextureBase
    {
    public:
        void Create(std::string srResourcePath) override;
        void ReCreate() override;
        void Update(uint32_t imageIndex) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Destroy() override;

        void Load();
    };
}