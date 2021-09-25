#pragma once

namespace Engine
{

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
}