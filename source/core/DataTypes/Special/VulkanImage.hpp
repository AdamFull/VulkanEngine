#pragma once

namespace Engine
{
    namespace Types
    {
        struct VulkanImage
        {
            vk::Image image;
            vk::DeviceMemory memory;
            vk::ImageView view;
        };
    }
}