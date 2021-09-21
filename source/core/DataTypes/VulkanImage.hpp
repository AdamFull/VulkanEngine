#pragma once

namespace Engine
{
    struct VulkanImage
    {
        vk::Image image;
        vk::DeviceMemory memory;
        vk::ImageView view;

        inline void Destroy(vk::UniqueDevice& device)
        {
            if(view)
                device.destroyImageView(view);
            
            if(image)
                device.destroyImage(image);

            if(memory)
                device.freeMemory(memory);
        }
    };
}