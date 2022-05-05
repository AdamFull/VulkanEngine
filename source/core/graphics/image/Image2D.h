#pragma once
#include "Image.h"

namespace Engine
{
    namespace Core
    {
        class CImage2D : public CImage
        {
        public:
            void create(const vk::Extent2D& extent, vk::Format format = vk::Format::eR8G8B8A8Unorm, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor,
            vk::Filter filter = vk::Filter::eLinear, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
            bool instantLayoutTransition = true, bool anisotropic = false, bool mipmaps = false);
        };
    }
}