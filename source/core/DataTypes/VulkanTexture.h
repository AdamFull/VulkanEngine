#pragma once

namespace Engine
{
    class VulkanHighLevel;

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

        void UpdateDescriptor();
        void Destroy(vk::UniqueDevice& device);
    };

    class VulkanTexture2D : public VulkanTextureBase
    {
    public:
        void LoadTextureFromFile(std::string, vk::ImageLayout layout, vk::UniqueDevice& device);
    };

    class VulkanCubemap : public VulkanTextureBase
    {
    public:

    };
}