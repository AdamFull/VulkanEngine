#pragma once
#include "VulkanImage.hpp"

namespace Engine
{
    namespace Types
    {
        namespace Texture
        {
            struct Base2D
            {
                VulkanImage info;
                vk::ImageLayout imageLayout;
                uint32_t width, height, channels;
                uint32_t mipLevels;
                uint32_t layerCount;
                vk::DescriptorImageInfo descriptor;
                vk::Sampler sampler;
            };

            void UpdateDescriptor(Base2D& texture);
            void Destroy(Main::FVulkanEngine& engine, Base2D& texture);
            void Load(Main::FVulkanEngine& engine, Base2D& texture, std::string srTexturePath, vk::ImageLayout layout);
        }
    }
}