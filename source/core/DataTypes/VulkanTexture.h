#pragma once

namespace Engine
{
    class Device;

    class VulkanTextureBase
    {
    public:
        vk::Image               image;
        vk::ImageLayout         imageLayout;
        vk::DeviceMemory        deviceMemory;
        vk::ImageView           view;
        uint32_t                width, height, channels;
        uint32_t                mipLevels;
        uint32_t                layerCount;
        vk::DescriptorImageInfo descriptor;
        vk::Sampler             sampler;

        void Load(std::unique_ptr<Device>& device, std::string srPath, uint32_t idx);

        void UpdateDescriptor();
        void Destroy(std::unique_ptr<Device>& device);
    private:
        void GenerateMipmaps(std::unique_ptr<Device>& device, vk::Image &image, uint32_t mipLevels, vk::Format format, vk::Extent3D sizes, vk::ImageAspectFlags aspectFlags);
    };
}